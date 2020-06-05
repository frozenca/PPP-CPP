#include <cassert>
#include <cstddef>
#include <atomic>
#include <memory>
#include <utility>
#include <tuple>
#include <functional>

template<typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    using element_type = T;
    using deleter_type = Deleter;
    using pointer = T *;
    static_assert(!std::is_rvalue_reference_v<Deleter>);

private:
    std::pair<pointer, deleter_type> ptr;

    template<std::enable_if_t<std::is_default_constructible_v<Deleter> && !std::is_pointer_v<Deleter>, bool>>
    constexpr UniquePtr() noexcept : ptr(pointer(), deleter_type{}) {}

    template<std::enable_if_t<std::is_default_constructible_v<Deleter> && !std::is_pointer_v<Deleter>, bool>>
    constexpr UniquePtr(std::nullptr_t) noexcept : ptr(pointer(), deleter_type{}) {}

    template<std::enable_if_t<std::is_default_constructible_v<Deleter> && !std::is_pointer_v<Deleter>, bool>>
    explicit UniquePtr(pointer p) noexcept : ptr(p, deleter_type{}) {}

    template<std::enable_if_t<std::is_constructible_v<Deleter, Deleter &>, bool>>
    UniquePtr(pointer p, Deleter &d) noexcept : ptr(p, d) {}

    template<std::enable_if_t<!std::is_reference_v<Deleter> && std::is_constructible_v<Deleter, Deleter &&>, bool>>
    UniquePtr(pointer p, Deleter &&d) noexcept : ptr(p, std::move(d)) {}

    UniquePtr(UniquePtr &&u) noexcept: ptr(u.release(), std::forward<Deleter>(u.getDeleter())) {}

    template<typename U, typename Deleter2,
            std::enable_if_t<std::is_convertible_v<U *, pointer> && !std::is_array_v<U>, bool>,
            std::enable_if_t<(std::is_reference_v<Deleter> && std::is_same_v<Deleter, Deleter2>)
                             || (!std::is_reference_v<Deleter> && std::is_convertible_v<Deleter2, Deleter>), bool>>
    UniquePtr(UniquePtr<U, Deleter2> &&u) noexcept : ptr(u.release(), std::forward<Deleter>(u.getDeleter())) {}

    UniquePtr &operator=(UniquePtr &&u) noexcept {
        reset(u.release());
        ptr.second = std::forward<Deleter>(u.getDeleter());
        return *this;
    }

    template<typename U, typename Deleter2,
            std::enable_if_t<std::is_convertible_v<U *, pointer> && !std::is_array_v<U>, bool>,
            std::enable_if_t<std::is_assignable_v<Deleter &, Deleter2 &&>, bool>>
    UniquePtr &operator=(UniquePtr<U, Deleter2> &&u) noexcept {
        reset(u.release());
        ptr.second = std::forward<Deleter2>(u.getDeleter());
        return *this;
    }

    ~UniquePtr() { reset(); }

    UniquePtr &operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    T &operator*() const {
        return *ptr.first;
    }

    pointer operator->() const noexcept {
        return ptr.first;
    }

    pointer get() const noexcept {
        return ptr.first;
    }

    deleter_type &getDeleter() noexcept {
        return ptr.second;
    }

    const deleter_type &getDeleter() const noexcept {
        return ptr.second;
    }

    explicit operator bool() const noexcept {
        return ptr.first != nullptr;
    }

    pointer release() noexcept {
        pointer t = ptr.first;
        ptr.first = pointer();
        return t;
    }

    void reset(pointer p = pointer()) noexcept {
        pointer t = ptr.first;
        ptr.first = p;
        if (t) {
            ptr.second(t);
        }
    }
};

class SharedCount {
    SharedCount(const SharedCount &);

    SharedCount &operator=(const SharedCount &);

protected:
    std::atomic<long> shared_owners;

    virtual ~SharedCount() = default;

private:
    virtual void onZeroShared() noexcept = 0;

public:
    explicit SharedCount(long refs = 0) noexcept: shared_owners(refs) {}

    void addShared() noexcept {
        shared_owners++;
    }

    bool releaseShared() noexcept {
        shared_owners--;
        if (shared_owners == -1) {
            onZeroShared();
            return true;
        }
        return false;
    }

    long useCount() const noexcept {
        return shared_owners + 1;
    }
};


class SharedWeakCount : private SharedCount {
    std::atomic<long> shared_weak_owners;
public:
    explicit SharedWeakCount(long refs = 0) noexcept: SharedCount(refs), shared_weak_owners(refs) {}

protected:
    ~SharedWeakCount() override = default;

public:
    void addShared() noexcept {
        SharedCount::addShared();
    }

    void addWeak() noexcept {
        shared_weak_owners++;
    }

    void releaseShared() noexcept {
        if (SharedCount::releaseShared()) {
            releaseWeak();
        }
    }

    void releaseWeak() noexcept;

    long useCount() const noexcept { return SharedCount::useCount(); }

    SharedWeakCount *lock() noexcept;

    virtual void onZeroSharedWeak() noexcept = 0;
};

void SharedWeakCount::releaseWeak() noexcept {
    if (std::atomic_load(&shared_weak_owners) == 0) {
        onZeroSharedWeak();
    } else if (--shared_weak_owners == -1) {
        onZeroSharedWeak();
    }
}

SharedWeakCount *SharedWeakCount::lock() noexcept {
    long object_owners = std::atomic_load(&shared_weak_owners);
    while (object_owners != -1) {
        if (std::atomic_compare_exchange_weak(&shared_owners, &object_owners, object_owners + 1)) {
            return this;
        }
    }
    return nullptr;
}


template<typename T, typename D, typename Alloc>
class SharedPtrPointer : public SharedWeakCount {
    std::pair<std::pair<T, D>, Alloc> data;
public:
    SharedPtrPointer(T p, D d, Alloc a) : data(std::pair<T, D>(p, std::move(d)), std::move(a)) {}

private:
    void onZeroShared() noexcept override;

    void onZeroSharedWeak() noexcept override;
};

template<typename T, typename D, typename Alloc>
void SharedPtrPointer<T, D, Alloc>::onZeroShared() noexcept {
    data.first.second(data.first.first);
    data.first.second.~D();
}

template<typename T, typename D, typename Alloc>
void SharedPtrPointer<T, D, Alloc>::onZeroSharedWeak() noexcept {
    using Al = typename Alloc::template rebind<SharedPtrPointer>::other;
    using ATraits = std::allocator_traits<Alloc>;
    using PTraits = std::pointer_traits<typename ATraits::pointer>;

    Al a(data.second);
    data.second.~Alloc();
    a.deallocate(PTraits::pointer_to(*this), 1);
}

template<typename T, typename Alloc>
class SharedPtrEmplace : public SharedWeakCount {
    std::pair<Alloc, T> data;
public:
    SharedPtrEmplace(Alloc a) : data(std::move(a), T()) {}

    template<typename... Args>
    SharedPtrEmplace(Alloc a, Args &&... args) :
            data(std::piecewise_construct, std::forward_as_tuple(a),
                 std::forward_as_tuple(std::forward<Args>(args)...)) {}

private:
    void onZeroShared() noexcept override;

    virtual void onZeroSharedWeak() noexcept;

public:
    T *get() noexcept { return std::addressof(data.second); }
};

template<typename T, typename Alloc>
void SharedPtrEmplace<T, Alloc>::onZeroShared() noexcept {
    data.second.~T();
}

template<typename T, typename Alloc>
void SharedPtrEmplace<T, Alloc>::onZeroSharedWeak() noexcept {
    using Al = typename Alloc::template rebind<SharedPtrPointer>::other;
    using ATraits = std::allocator_traits<Alloc>;
    using PTraits = std::pointer_traits<typename ATraits::pointer>;

    Al a(data.first);
    data.first.~Alloc();
    a.deallocate(PTraits::pointer_to(*this), 1);
}


template<typename T>
class WeakPtr;

template<typename T>
class enableSharedFromThis;

template<typename T>
class SharedPtr {
public:
    using element_type = T;
    using weak_type = WeakPtr<T>;

private:
    element_type *ptr;
    SharedWeakCount *cntrl;

public:
    constexpr SharedPtr() noexcept;

    constexpr SharedPtr(std::nullptr_t) noexcept;

    template<typename U, std::enable_if_t<std::is_convertible_v<U *, element_type *>, bool> = false>
    explicit SharedPtr(U *p);

    template<typename U, typename D, std::enable_if_t<std::is_convertible_v<U *, element_type *>, bool> = false>
    SharedPtr(U *p, D d);

    template<typename U, typename D, typename Alloc, std::enable_if_t<std::is_convertible_v<U *, element_type *>, bool> = false>
    SharedPtr(U *p, D d, Alloc a);

    template<typename D>
    SharedPtr(std::nullptr_t, D d);

    template<typename D, typename Alloc>
    SharedPtr(std::nullptr_t, D d, Alloc a);

    template<typename U>
    SharedPtr(const SharedPtr<U> &r, element_type *p) noexcept;

    SharedPtr(const SharedPtr &r) noexcept;

    template<typename U, std::enable_if_t<std::is_convertible_v<U *, element_type *>, bool> = false>
    SharedPtr(const SharedPtr<U> &r) noexcept;

    SharedPtr(SharedPtr &&r) noexcept;

    template<typename U, std::enable_if_t<std::is_convertible_v<U *, element_type *>, bool> = false>
    SharedPtr(SharedPtr<U> &&r) noexcept;

    template<typename U, std::enable_if_t<std::is_convertible_v<U *, element_type *>, bool> = false>
    SharedPtr(const WeakPtr<U> &r) noexcept;

    template<typename U, typename Deleter, std::enable_if_t<!std::is_lvalue_reference_v<Deleter> &&
                                                            !std::is_array_v<U> &&
                                                            std::is_convertible_v<U *, element_type *>, bool> = false>
    SharedPtr(UniquePtr<U, Deleter> &&);

    template<typename U, typename Deleter, std::enable_if_t<std::is_lvalue_reference_v<Deleter> &&
                                                            !std::is_array_v<U> &&
                                                            std::is_convertible_v<U *, element_type *>, bool> = false>
    SharedPtr(UniquePtr<U, Deleter> &&);

    ~SharedPtr();

    SharedPtr &operator=(const SharedPtr &r) noexcept;

    template<typename U>
    typename std::enable_if_t<std::is_convertible_v<U *, T *>, SharedPtr &>
    operator=(const SharedPtr<U> &r) noexcept;

    SharedPtr &operator=(SharedPtr &&r) noexcept;

    template<typename U>
    typename std::enable_if_t<std::is_convertible_v<U *, T *>, SharedPtr &>
    operator=(SharedPtr<U> &&r) noexcept;

    template<typename U, typename Deleter>
    typename std::enable_if_t<!std::is_array_v<U> &&
                              std::is_convertible_v<U *, element_type *>, SharedPtr &>
    operator=(UniquePtr<U, Deleter> &&r) noexcept;

    void swap(SharedPtr &r) noexcept;

    void reset() noexcept;

    template<typename U>
    typename std::enable_if_t<std::is_convertible_v<U *, T *>, void>
    reset(U *p) noexcept;

    template<typename U, typename Deleter>
    typename std::enable_if_t<std::is_convertible_v<U *, T *>, void>
    reset(U *p, Deleter d) noexcept;

    template<typename U, typename Deleter, typename Alloc>
    typename std::enable_if_t<std::is_convertible_v<U *, T *>, void>
    reset(U *p, Deleter d, Alloc a) noexcept;

    element_type *get() const noexcept { return ptr; }

    element_type &operator*() const noexcept { return *ptr; }

    element_type *operator->() const noexcept { return ptr; }

    long useCount() const noexcept {
        return cntrl ? cntrl->useCount() : 0;
    }

    bool unique() const noexcept {
        return useCount() == 1;
    }

    explicit operator bool() const noexcept {
        return get() != 0;
    }

    template<typename U>
    bool ownerBefore(SharedPtr<U> const &p) const noexcept {
        return cntrl < p.cntrl;
    }

    template<typename U>
    bool ownerBefore(WeakPtr<U> const &p) const noexcept {
        return cntrl < p.cntrl;
    }

    bool ownerEquivalent(SharedPtr &p) const {
        return cntrl == p.cntrl;
    }

    template<typename U, typename ControlBlock>
    static SharedPtr<T> createWithControlBlock(U *p, ControlBlock *cntrl) {
        SharedPtr<T> r;
        r.ptr = p;
        r.cntrl = cntrl;
        r.enableWeakThis(r.ptr, r.ptr);
        return r;
    }

    template<typename Alloc, typename... Args>
    static SharedPtr<T> allocateShared(const Alloc &a, Args &&... args);

private:
    template<typename U, typename OrigPtr>
    typename std::enable_if_t<std::is_convertible_v<OrigPtr *, const enableSharedFromThis<U> *>, void>
    enableWeakThis(const enableSharedFromThis<U> *e, OrigPtr *ptr) noexcept {
        using RawU = std::remove_cv<U>;
        if (e && e->weak_this.expired()) {
            e->weak_this = SharedPtr<RawU>(*this, const_cast<RawU *>(static_cast<const U *>(ptr)));
        }
    }

    void enableWeakThis(...) noexcept {}

    template<typename U>
    friend
    class SharedPtr;

    template<typename U>
    friend
    class WeakPtr;
};

template<typename T>
inline constexpr SharedPtr<T>::SharedPtr() noexcept : ptr(0), cntrl(nullptr) {}

template<typename T>
inline constexpr SharedPtr<T>::SharedPtr(std::nullptr_t) noexcept : ptr(0), cntrl(nullptr) {}

template<typename T>
template<typename U, std::enable_if_t<std::is_convertible_v<U *, T *>, bool>>
SharedPtr<T>::SharedPtr(U *p) : ptr(p) {
    UniquePtr<U> hold(p);
    using Alloc = typename std::allocator<U>;
    using ControlBlock = SharedPtrPointer<U, std::default_delete<U>, Alloc>;
    cntrl = new ControlBlock(p, std::default_delete<U>(), Alloc());
    hold.release();
    enableWeakThis(p, p);
}

template<typename T>
template<typename U, typename Deleter, std::enable_if_t<std::is_convertible_v<U *, T *>, bool>>
SharedPtr<T>::SharedPtr(U *p, Deleter d) : ptr(p) {
    try {
        using Alloc = typename std::allocator<U>;
        using ControlBlock = SharedPtrPointer<U, std::default_delete<U>, Alloc>;
        cntrl = new ControlBlock(p, std::default_delete<U>(), Alloc());
        enableWeakThis(p, p);
    } catch (...) {
        d(p);
        throw;
    }
}

template<typename T>
template<typename Deleter>
SharedPtr<T>::SharedPtr(std::nullptr_t p, Deleter d) : ptr(0) {
    try {
        using Alloc = typename std::allocator<T>;
        using ControlBlock = SharedPtrPointer<std::nullptr_t, Deleter, Alloc>;
        cntrl = new ControlBlock(p, d, Alloc());
    } catch (...) {
        d(p);
        throw;
    }
}

template<typename T>
template<typename Deleter, typename Alloc>
SharedPtr<T>::SharedPtr(std::nullptr_t p, Deleter d, Alloc a) : ptr(0) {
    try {
        using ControlBlock = SharedPtrPointer<std::nullptr_t, Deleter, Alloc>;
        using Alloc2 = typename Alloc::template rebind<ControlBlock>::other;
        Alloc2 a2(a);
        UniquePtr<ControlBlock, Deleter> hold2(a2.allocate(1), Deleter(a2, 1));
        ::new(static_cast<void *>(std::addressof(*hold2.get()))) ControlBlock(p, d, a);
        cntrl = std::addressof(*hold2.release());
        enableWeakThis(p, p);
    } catch (...) {
        d(p);
        throw;
    }
}

template<typename T>
template<typename U, typename Deleter, typename Alloc, std::enable_if_t<std::is_convertible_v<U *, T *>, bool>>
SharedPtr<T>::SharedPtr(U *p, Deleter d, Alloc a) : ptr(p) {
    try {
        using ControlBlock = SharedPtrPointer<U, Deleter, Alloc>;
        using Alloc2 = typename Alloc::template rebind<ControlBlock>::other;
        Alloc2 a2(a);
        UniquePtr<ControlBlock, Deleter> hold2(a2.allocate(1), Deleter(a2, 1));
        ::new(static_cast<void *>(std::addressof(*hold2.get()))) ControlBlock(p, d, a);
        cntrl = std::addressof(*hold2.release());
        enableWeakThis(p, p);
    } catch (...) {
        d(p);
        throw;
    }
}

template<typename T>
template<typename U>
inline SharedPtr<T>::SharedPtr(const SharedPtr<U> &r, T *p) noexcept : ptr(p), cntrl(r.cntrl) {
    if (cntrl) {
        cntrl->addShared();
    }
}

template<typename T>
inline SharedPtr<T>::SharedPtr(const SharedPtr &r) noexcept : ptr(r.ptr), cntrl(r.cntrl) {
    if (cntrl) {
        cntrl->addShared();
    }
}

template<typename T>
inline SharedPtr<T>::SharedPtr(SharedPtr &&r) noexcept : ptr(r.ptr), cntrl(r.cntrl) {
    r.ptr = 0;
    r.cntrl = 0;
}

template<typename T>
template<typename U, std::enable_if_t<std::is_convertible_v<U *, T *>, bool>>
inline SharedPtr<T>::SharedPtr(SharedPtr<U> &&r) noexcept : ptr(r.ptr), cntrl(r.cntrl) {
    r.ptr = 0;
    r.cntrl = 0;
}

template<typename T>
template<typename U, typename Deleter, std::enable_if_t<!std::is_lvalue_reference_v<Deleter> && !std::is_array_v<U>
                                                        && std::is_convertible_v<U *, T *>, bool>>
SharedPtr<T>::SharedPtr(UniquePtr<U, Deleter> &&r) : ptr(r.get()) {
    if (ptr == nullptr) {
        cntrl = nullptr;
    } else {
        using Alloc = std::allocator<U>;
        using ControlBlock = SharedPtrPointer<U *, Deleter, Alloc>;
        cntrl = new ControlBlock(r.get(), r.getDeleter(), Alloc());
        enableWeakThis(r.get(), r.get());
    }
    r.release();
}

template<typename T>
template<typename U, typename Deleter, std::enable_if_t<std::is_lvalue_reference_v<Deleter> && !std::is_array_v<U>
                                                        && std::is_convertible_v<U *, T *>, bool>>
SharedPtr<T>::SharedPtr(UniquePtr<U, Deleter> &&r) : ptr(r.get()) {
    if (ptr == nullptr) {
        cntrl = nullptr;
    } else {
        using Alloc = std::allocator<U>;
        using ControlBlock = SharedPtrPointer<U *, std::reference_wrapper<std::remove_reference_t<Deleter>>, Alloc>;
        cntrl = new ControlBlock(r.get(), std::ref(r.getDeleter()), Alloc());
        enableWeakThis(r.get(), r.get());
    }
    r.release();
}

template<typename T>
template<typename Alloc, typename... Args>
SharedPtr<T> SharedPtr<T>::allocateShared(const Alloc &a, Args &&... args) {
    static_assert(std::is_constructible_v<T, Args...>);
    using ControlBlock = SharedPtrEmplace<T, Alloc>;
    using Alloc2 = typename Alloc::template rebind<ControlBlock>::other;
    Alloc2 a2(a);
    UniquePtr<ControlBlock, std::default_delete<T>> hold2(a2.allocate(1), Deleter(a2, 1));
    ::new(static_cast<void *>(std::addressof(*hold2.get()))) ControlBlock(a, std::forward<Args>(args)...);
    SharedPtr<T> r;
    r.ptr = hold2.get()->get();
    r.cntrl = std::addressof(*hold2.release());
    r.enableWeakThis(r.ptr, r.ptr);
    return r;
}

template<typename T>
SharedPtr<T>::~SharedPtr<T>() {
    if (cntrl) {
        cntrl->releaseShared();
    }
};

template<typename T>
inline SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr &r) noexcept {
    SharedPtr(r).swap(*this);
    return *this;
}

template<typename T>
template<typename U>
inline typename std::enable_if_t<std::is_convertible_v<U *, T *>, SharedPtr<T> &>
SharedPtr<T>::operator=(const SharedPtr<U> &r) noexcept {
    SharedPtr(r).swap(*this);
    return *this;
}

template<typename T>
inline SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr &&r) noexcept {
    SharedPtr(std::move(r)).swap(*this);
    return *this;
}

template<typename T>
template<typename U>
inline typename std::enable_if_t<std::is_convertible_v<U *, T *>, SharedPtr<T> &>
SharedPtr<T>::operator=(SharedPtr<U> &&r) noexcept {
    SharedPtr(std::move(r)).swap(*this);
    return *this;
}

template<typename T>
template<typename U, typename Deleter>
typename std::enable_if_t<!std::is_array_v<U> &&
                          std::is_convertible_v<U *, T *>, SharedPtr<T> &>
SharedPtr<T>::operator=(UniquePtr<U, Deleter> &&r) noexcept {
    SharedPtr(std::move(r)).swap(*this);
    return *this;
}

template<typename T>
inline void SharedPtr<T>::swap(SharedPtr &r) noexcept {
    std::swap(ptr, r.ptr);
    std::swap(cntrl, r.cntrl);
}

template<typename T>
inline void SharedPtr<T>::reset() noexcept {
    SharedPtr().swap(*this);
}

template<typename T>
template<typename U>
inline typename std::enable_if_t<std::is_convertible_v<U *, T *>, void>
SharedPtr<T>::reset(U *p) noexcept {
    SharedPtr(p).swap(*this);
}

template<typename T>
template<typename U, typename Deleter>
inline typename std::enable_if_t<std::is_convertible_v<U *, T *>, void>
SharedPtr<T>::reset(U *p, Deleter d) noexcept {
    SharedPtr(p, d).swap(*this);
}

template<typename T>
template<typename U, typename Deleter, typename Alloc>
inline typename std::enable_if_t<std::is_convertible_v<U *, T *>, void>
SharedPtr<T>::reset(U *p, Deleter d, Alloc a) noexcept {
    SharedPtr(p, d, a).swap(*this);
}

template<typename T, typename... Args>
inline typename std::enable_if_t<!std::is_array_v<T>, SharedPtr<T>>
makeShared(Args &&... args) {
    static_assert(std::is_constructible_v<T, Args...>);
    using ControlBlock = SharedPtrEmplace<T, std::allocator<T>>;
    using Alloc2 = std::allocator<ControlBlock>;
    Alloc2 a2;
    UniquePtr<ControlBlock, std::default_delete<T>> hold2(a2.allocate(1), Deleter(a2, 1));
    ::new(hold2.get()) ControlBlock(a2, std::forward<Args>(args)...);
    T *ptr = hold2.get()->get();
    return SharedPtr<T>::createWithControlBlock(ptr, hold2.release());
}

template<typename T, typename Alloc, typename... Args>
inline typename std::enable_if_t<!std::is_array_v<T>, SharedPtr<T>>
allocateShared(const Alloc &a, Args &&... args) {
    return SharedPtr<T>::allocateShared(a, std::forward<Args>(args)...);
}

template<typename T, typename U>
inline bool operator==(const SharedPtr<T> &x, const SharedPtr<U> &y) noexcept {
    return x.get() == y.get();
}

template<typename T, typename U>
inline bool operator!=(const SharedPtr<T> &x, const SharedPtr<U> &y) noexcept {
    return !(x == y);
}

template<typename T, typename U>
inline bool operator<(const SharedPtr<T> &x, const SharedPtr<U> &y) noexcept {
    return std::less<>()(x.get(), y.get());
}

template<typename T, typename U>
inline bool operator>(const SharedPtr<T> &x, const SharedPtr<U> &y) noexcept {
    return y < x;
}

template<typename T, typename U>
inline bool operator<=(const SharedPtr<T> &x, const SharedPtr<U> &y) noexcept {
    return !(y < x);
}

template<typename T, typename U>
inline bool operator>=(const SharedPtr<T> &x, const SharedPtr<U> &y) noexcept {
    return !(x > y);
}

template<typename T>
inline bool operator==(const SharedPtr<T> &x, std::nullptr_t) noexcept {
    return !x;
}

template<typename T>
inline bool operator==(std::nullptr_t, const SharedPtr<T> &x) noexcept {
    return !x;
}

template<typename T>
inline bool operator!=(const SharedPtr<T> &x, std::nullptr_t) noexcept {
    return static_cast<bool>(x);
}

template<typename T>
inline bool operator!=(std::nullptr_t, const SharedPtr<T> &x) noexcept {
    return static_cast<bool>(x);
}

template<typename T>
inline bool operator<(const SharedPtr<T> &x, std::nullptr_t) noexcept {
    return std::less<T *>()(x.get(), nullptr);
}

template<typename T>
inline bool operator<(std::nullptr_t, const SharedPtr<T> &x) noexcept {
    return std::less<T *>()(nullptr, x.get());
}

template<typename T>
inline bool operator>(const SharedPtr<T> &x, std::nullptr_t) noexcept {
    return nullptr < x;
}

template<typename T>
inline bool operator>(std::nullptr_t, const SharedPtr<T> &x) noexcept {
    return x < nullptr;
}

template<typename T>
inline bool operator<=(const SharedPtr<T> &x, std::nullptr_t) noexcept {
    return !(nullptr < x);
}

template<typename T>
inline bool operator<=(std::nullptr_t, const SharedPtr<T> &x) noexcept {
    return !(x < nullptr);
}

template<typename T>
inline bool operator>=(const SharedPtr<T> &x, std::nullptr_t) noexcept {
    return !(x < nullptr);
}

template<typename T>
inline bool operator>=(std::nullptr_t, const SharedPtr<T> &x) noexcept {
    return !(nullptr < x);
}

template<typename Deleter, typename T>
inline Deleter *getDeleter(const SharedPtr<T> &p) noexcept {
    return p.template getDeleter<Deleter>();
}

template<typename T>
class WeakPtr {
public:
    using element_type = T;
private:
    element_type *ptr;
    SharedWeakCount *cntrl;
public:
    constexpr WeakPtr() noexcept;

    template<typename Y, std::enable_if_t<std::is_convertible_v<Y *, T *>, bool> = false>
    WeakPtr(SharedPtr<Y> const &r) noexcept;

    WeakPtr(WeakPtr const &r) noexcept;

    template<typename Y, std::enable_if_t<std::is_convertible_v<Y *, T *>, bool> = false>
    WeakPtr(WeakPtr<Y> const &r) noexcept;

    WeakPtr(WeakPtr &&r) noexcept;

    template<typename Y, std::enable_if_t<std::is_convertible_v<Y *, T *>, bool> = false>
    WeakPtr(WeakPtr<Y> &&r) noexcept;

    ~WeakPtr();

    WeakPtr &operator=(WeakPtr const &r) noexcept;

    template<typename Y>
    typename std::enable_if_t<std::is_convertible_v<Y *, element_type *>, WeakPtr &>
    operator=(WeakPtr<Y> const &r) noexcept;

    WeakPtr &operator=(WeakPtr &&r) noexcept;

    template<typename Y>
    typename std::enable_if_t<std::is_convertible_v<Y *, element_type *>, WeakPtr &>
    operator=(WeakPtr<Y> &&r) noexcept;

    template<typename Y>
    typename std::enable_if_t<std::is_convertible_v<Y *, element_type *>, WeakPtr &>
    operator=(SharedPtr<Y> const &r) noexcept;

    void swap(WeakPtr &r) noexcept;

    void reset() noexcept;

    long useCount() const noexcept {
        return cntrl ? cntrl->useCount() : 0;
    }

    bool expired() const noexcept {
        return cntrl == nullptr || cntrl->useCount() == 0;
    }

    SharedPtr<T> lock() const noexcept;

    template<typename U>
    bool ownerBefore(const SharedPtr<U> &r) const noexcept {
        return cntrl < r.cntrl;
    }

    template<typename U>
    bool ownerBefore(const WeakPtr<U> &r) const noexcept {
        return cntrl < r.cntrl;
    }

    template<typename U> friend
    class WeakPtr;

    template<typename U> friend
    class SharedPtr;
};

template<typename T>
class enableSharedFromThis {
    mutable WeakPtr<T> weak_this;
protected:
    enableSharedFromThis() noexcept = default;

    enableSharedFromThis(enableSharedFromThis const &) noexcept {}

    enableSharedFromThis &operator=(enableSharedFromThis const &) noexcept {
        return *this;
    }

    ~enableSharedFromThis() = default;

public:
    SharedPtr<T> sharedFromThis() {
        return SharedPtr<T>(weak_this);
    }

    SharedPtr<T const> sharedFromThis() const {
        return SharedPtr<const T>(weak_this);
    }

    WeakPtr<T> weakFromThis() noexcept {
        return weak_this;
    }

    WeakPtr<const T> weakFromThis() const noexcept {
        return weak_this;
    }

    template<typename U>
    friend
    class SharedPtr;
};

template<typename T>
inline constexpr WeakPtr<T>::WeakPtr() noexcept : ptr(0), cntrl(nullptr) {}

template<typename T>
inline WeakPtr<T>::WeakPtr(WeakPtr const &r) noexcept : ptr(r.ptr), cntrl(r.cntrl) {
    if (cntrl) {
        cntrl->addWeak();
    }
}

template<typename T>
template<typename U, std::enable_if_t<std::is_convertible_v<U *, T *>, bool>>
inline WeakPtr<T>::WeakPtr(SharedPtr<U> const &r) noexcept : ptr(r.ptr), cntrl(r.cntrl) {
    if (cntrl) {
        cntrl->addWeak();
    }
}

template<typename T>
template<typename U, std::enable_if_t<std::is_convertible_v<U *, T *>, bool>>
inline WeakPtr<T>::WeakPtr(WeakPtr<U> const &r) noexcept : ptr(r.ptr), cntrl(r.cntrl) {
    if (cntrl) {
        cntrl->addWeak();
    }
}

template<typename T>
inline WeakPtr<T>::WeakPtr(WeakPtr &&r) noexcept : ptr(r.ptr), cntrl(r.cntrl) {
    r.ptr = nullptr;
    r.cntrl = nullptr;
}

template<typename T>
template<typename U, std::enable_if_t<std::is_convertible_v<U *, T *>, bool>>
inline WeakPtr<T>::WeakPtr(WeakPtr<U> &&r) noexcept : ptr(r.ptr), cntrl(r.cntrl) {
    r.ptr = nullptr;
    r.cntrl = nullptr;
}

template<typename T>
WeakPtr<T>::~WeakPtr() {
    if (cntrl) {
        cntrl->releaseWeak();
    }
}

template<typename T>
inline WeakPtr<T> &WeakPtr<T>::operator=(WeakPtr<T> const &r) noexcept {
    WeakPtr(r).swap(*this);
    return *this;
}

template<typename T>
template<typename Y>
typename std::enable_if_t<std::is_convertible_v<Y *, T *>, WeakPtr<T> &>
WeakPtr<T>::operator=(WeakPtr<Y> const &r) noexcept {
    WeakPtr(r).swap(*this);
    return *this;
}

template<typename T>
inline WeakPtr<T> &WeakPtr<T>::operator=(WeakPtr &&r) noexcept {
    WeakPtr(std::move(r)).swap(*this);
    return *this;
}

template<typename T>
template<typename Y>
typename std::enable_if_t<std::is_convertible_v<Y *, T *>, WeakPtr<T> &>
WeakPtr<T>::operator=(WeakPtr<Y> &&r) noexcept {
    WeakPtr(std::move(r)).swap(*this);
    return *this;
}

template<typename T>
template<typename Y>
typename std::enable_if_t<std::is_convertible_v<Y *, T *>, WeakPtr<T> &>
WeakPtr<T>::operator=(SharedPtr<Y> const &r) noexcept {
    WeakPtr(r).swap(*this);
    return *this;
}

template<typename T>
inline void WeakPtr<T>::swap(WeakPtr<T> &r) noexcept {
    std::swap(ptr, r.ptr);
    std::swap(cntrl, r.cntrl);
}

template<typename T>
inline void WeakPtr<T>::reset() noexcept {
    WeakPtr().swap(*this);
}

template<typename T>
SharedPtr<T> WeakPtr<T>::lock() const noexcept {
    SharedPtr<T> r;
    r.cntrl == cntrl ? cntrl->lock() : cntrl;
    if (r.cntrl) {
        r.ptr = ptr;
    }
    return r;
}


int main() {
}