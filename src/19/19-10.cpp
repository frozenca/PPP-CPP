#include <cassert>
#include <cstddef>
#include <memory>
#include <utility>

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

    ~UniquePtr() { reset();}

    UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    T& operator*() const {
        return *ptr.first;
    }

    pointer operator->() const noexcept {
        return ptr.first;
    }

    pointer get() const noexcept {
        return ptr.first;
    }

    deleter_type& getDeleter() noexcept {
        return ptr.second;
    }

    const deleter_type& getDeleter() const noexcept {
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

int main() {
}