#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename T, typename Allocator>
class VectorBase {
public:
    using allocator_type = Allocator;
    using AllocTraits = std::allocator_traits<allocator_type>;
    using size_type = typename AllocTraits::size_type;
protected:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using difference_type = typename AllocTraits::difference_type;
    using pointer = typename AllocTraits::pointer;
    using const_pointer = typename AllocTraits::const_pointer;
    using iterator = pointer;
    using const_iterator = const_pointer;

    pointer begin_;
    pointer end_;

    std::pair<pointer, allocator_type> end_cap_;

    allocator_type& alloc() noexcept { return end_cap_.second;}
    const allocator_type& alloc() const noexcept { return end_cap_.second;}
    pointer& endCap() noexcept {return end_cap_.first;}
    const pointer& endCap() const noexcept {return end_cap_.first;}

    VectorBase() noexcept(std::is_nothrow_default_constructible_v<allocator_type>);
    VectorBase(const allocator_type& a);
    VectorBase(allocator_type&& a) noexcept;
    ~VectorBase();

    void clear() noexcept { destructAtEnd(begin_);}
    size_type capacity() const noexcept { return static_cast<size_type>(endCap() - begin_);}

    void destructAtEnd(pointer newLast) noexcept;

    void copyAssignAlloc(const VectorBase& c) {
        copyAssignAlloc(c, std::integral_constant<bool, AllocTraits::propagate_on_container_copy_assignment::value>());
    }

    void moveAssignAlloc(VectorBase& c) noexcept(!AllocTraits::propagate_on_container_move_assignment::value ||
    std::is_nothrow_move_assignable_v<allocator_type>) {
        moveAssignAlloc(c, std::integral_constant<bool, AllocTraits::propagate_on_container_move_assignment::value>());
    }

private:
    void copyAssignAlloc(const VectorBase& c, std::true_type) {
        if (alloc() != c.alloc()) {
            clear();
            AllocTraits::deallocate(alloc(), begin_, capacity());
            begin_ = end_ = endCap() = nullptr;
        }
        alloc() = c.alloc();
    }

    void copyAssignAlloc(const VectorBase& c, std::false_type) {}

    void moveAssignAlloc(VectorBase& c, std::true_type) noexcept (std::is_nothrow_move_assignable_v<allocator_type>) {
        alloc() = std::move(c.alloc());
    }

    void moveAssignAlloc(VectorBase&, std::false_type) {}
};

template <typename T, typename Allocator>
inline void VectorBase<T, Allocator>::destructAtEnd(pointer newLast) noexcept {
    pointer soonToBeEnd = end_;
    while (newLast != soonToBeEnd) {
        AllocTraits::destroy(alloc(), std::to_address(--soonToBeEnd));
    }
    end_ = newLast;
}

template <typename T, typename Allocator>
inline VectorBase<T, Allocator>::VectorBase() noexcept (std::is_nothrow_default_constructible_v<allocator_type>)
: begin_(nullptr), end_(nullptr), end_cap_(nullptr, Allocator()){
}

template <typename T, typename Allocator>
inline VectorBase<T, Allocator>::VectorBase(const allocator_type& a)
        : begin_(nullptr), end_(nullptr), end_cap_(nullptr, a){
}

template <typename T, typename Allocator>
inline VectorBase<T, Allocator>::VectorBase(allocator_type&& a) noexcept
        : begin_(nullptr), end_(nullptr), end_cap_(nullptr, std::move(a)) {
}

template <typename T, typename Allocator>
VectorBase<T, Allocator>::~VectorBase() {
    if (begin_ != nullptr) {
        clear();
        AllocTraits::deallocate(alloc(), begin_, capacity());
    }
}

template <typename Iter>
class WrapIter {
public:
    using iterator_type = Iter;
    using iterator_category = typename std::iterator_traits<iterator_type>::iterator_category;
    using value_type = typename std::iterator_traits<iterator_type>::value_type;
    using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
    using pointer = typename std::iterator_traits<iterator_type>::pointer;
    using reference = typename std::iterator_traits<iterator_type>::reference;
private:
    iterator_type i;
public:
    constexpr WrapIter() noexcept : i {} {}

    template <typename U>
    WrapIter(const WrapIter<U>& u, typename std::enable_if_t<std::is_convertible_v<U, iterator_type>>* = 0) noexcept
    : i (u.base()) {}

    constexpr reference operator*() const noexcept { return *i; }
    constexpr pointer operator->() const noexcept { return (pointer) std::addressof(*i); }
    constexpr WrapIter& operator++() noexcept {
        ++i;
        return *this;
    }
    constexpr WrapIter operator++(int) noexcept {
        WrapIter tmp(*this);
        ++(*this);
        return tmp;
    }
    constexpr WrapIter& operator--() noexcept {
        --i;
        return *this;
    }
    constexpr WrapIter operator--(int) noexcept {
        WrapIter tmp(*this);
        --(*this);
        return tmp;
    }
    constexpr WrapIter operator+(difference_type n) const noexcept {
        WrapIter w(*this);
        w += n;
        return w;
    }
    constexpr WrapIter& operator+=(difference_type n) const noexcept {
        i += n;
        return *this;
    }
    constexpr WrapIter operator-(difference_type n) const noexcept {
        return *this + (-n);
    }
    constexpr WrapIter& operator-=(difference_type n) const noexcept {
        *this += -n;
        return *this;
    }
    constexpr reference operator[](difference_type n) const noexcept {
        return i[n];
    }
    constexpr iterator_type base() const noexcept { return i; }

private:
    constexpr WrapIter(iterator_type x) noexcept : i(x) {}

    template <typename U> friend class WrapIter;
    template <typename T, typename Alloc> friend class Vector;

    template <typename Iter1, typename Iter2>
    constexpr friend bool operator==(const WrapIter<Iter1>&, const WrapIter<Iter2>&) noexcept;

    template <typename Iter1, typename Iter2>
    constexpr friend bool operator<(const WrapIter<Iter1>&, const WrapIter<Iter2>&) noexcept;

    template <typename Iter1, typename Iter2>
    constexpr friend bool operator!=(const WrapIter<Iter1>&, const WrapIter<Iter2>&) noexcept;

    template <typename Iter1, typename Iter2>
    constexpr friend bool operator>(const WrapIter<Iter1>&, const WrapIter<Iter2>&) noexcept;

    template <typename Iter1, typename Iter2>
    constexpr friend bool operator>=(const WrapIter<Iter1>&, const WrapIter<Iter2>&) noexcept;

    template <typename Iter1, typename Iter2>
    constexpr friend bool operator<=(const WrapIter<Iter1>&, const WrapIter<Iter2>&) noexcept;

    template <typename Iter1, typename Iter2>
    constexpr friend auto operator-(const WrapIter<Iter1>& x, const WrapIter<Iter2>& y) noexcept -> decltype(x.base() - y.base());

    template <typename Iter1>
    constexpr friend WrapIter<Iter1> operator+(typename WrapIter<Iter1>::difference_type, WrapIter<Iter1>) noexcept;

    template <typename Inp, typename Outp>
    friend constexpr Outp copy(Inp, Inp, Outp);

    template <typename B1, typename B2>
    friend constexpr B2 copyBackward(B1, B1, B2);

    template <typename Inp, typename Outp>
    friend Outp move(Inp, Inp, Outp);

    template <typename B1, typename B2>
    friend B2 moveBackward(B1, B1, B2);

    template <typename T>
    constexpr friend typename std::enable_if_t<std::is_trivially_copy_assignable_v<T>, T*> UnwrapIter(WrapIter<T*>);

};

template <typename Iter1, typename Iter2>
inline constexpr bool operator==(const WrapIter<Iter1>& x, const WrapIter<Iter2>& y) noexcept {
    return x.base() == y.base();
}

template <typename Iter1, typename Iter2>
inline constexpr bool operator<(const WrapIter<Iter1>& x, const WrapIter<Iter2>& y) noexcept {
    return x.base() < y.base();
}

template <typename Iter1, typename Iter2>
inline constexpr bool operator!=(const WrapIter<Iter1>& x, const WrapIter<Iter2>& y) noexcept {
    return !(x == y);
}

template <typename Iter1, typename Iter2>
inline constexpr bool operator>(const WrapIter<Iter1>& x, const WrapIter<Iter2>& y) noexcept {
    return y < x;
}

template <typename Iter1, typename Iter2>
inline constexpr bool operator>=(const WrapIter<Iter1>& x, const WrapIter<Iter2>& y) noexcept {
    return !(x < y);
}

template <typename Iter1, typename Iter2>
inline constexpr bool operator<=(const WrapIter<Iter1>& x, const WrapIter<Iter2>& y) noexcept {
    return !(y < x);
}

template <typename Iter1>
inline constexpr bool operator!=(const WrapIter<Iter1>& x, const WrapIter<Iter1>& y) noexcept {
    return !(x == y);
}

template <typename Iter1>
inline constexpr bool operator>(const WrapIter<Iter1>& x, const WrapIter<Iter1>& y) noexcept {
    return y < x;
}

template <typename Iter1>
inline constexpr bool operator>=(const WrapIter<Iter1>& x, const WrapIter<Iter1>& y) noexcept {
    return !(x < y);
}

template <typename Iter1>
inline constexpr bool operator<=(const WrapIter<Iter1>& x, const WrapIter<Iter1>& y) noexcept {
    return !(y < x);
}

template <typename Iter1, typename Iter2>
inline constexpr auto operator-(const WrapIter<Iter1>& x, const WrapIter<Iter2>& y) noexcept -> decltype(x.base() - y.base()) {
    return x.base() - y.base();
}

template <typename Iter1>
constexpr WrapIter<Iter1> operator+(typename WrapIter<Iter1>::difference_type n, WrapIter<Iter1> x) noexcept {
    x += n;
    return x;
}

template <typename T, typename Allocator = std::allocator<T>>
struct SplitBuffer {
private:
    SplitBuffer(const SplitBuffer&);
    SplitBuffer& operator=(const SplitBuffer&);
public:
    using value_type = T;
    using allocator_type = Allocator;
    using AllocRR = typename std::remove_reference_t<allocator_type>;
    using AllocTraits = std::allocator_traits<AllocRR>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = typename AllocTraits::size_type;
    using difference_type = typename AllocTraits::difference_type;
    using pointer = typename AllocTraits::pointer;
    using const_pointer = typename AllocTraits::const_pointer;
    using iterator = pointer;
    using const_iterator = const_pointer;

    pointer first_;
    pointer begin_;
    pointer end_;
    std::pair<pointer, allocator_type> end_cap_;

    using AllocRef = typename std::add_lvalue_reference_t<allocator_type>;
    using AllocConstRef = typename std::add_lvalue_reference_t<allocator_type>;

    AllocRR& Alloc() noexcept { return end_cap_.second; }
    const AllocRR& Alloc() const noexcept { return end_cap_.second; }
    pointer& EndCap() noexcept { return end_cap_.first; }
    const pointer& EndCap() const noexcept { return end_cap_.first; }
    SplitBuffer() noexcept(std::is_nothrow_default_constructible_v<allocator_type>);
    explicit SplitBuffer(AllocRR& a);
    explicit SplitBuffer(const AllocRR& a);
    SplitBuffer(size_type cap, size_type start, AllocRR& a);
    ~SplitBuffer();
    SplitBuffer(SplitBuffer&& c) noexcept (std::is_nothrow_move_constructible_v<allocator_type>);
    SplitBuffer(SplitBuffer&& c, const AllocRR& a);
    SplitBuffer& operator=(SplitBuffer&& c) noexcept ((AllocTraits::propagate_on_container_move_assignment::value
            && std::is_nothrow_move_assignable_v<allocator_type>) || !AllocTraits::propagate_on_container_move_assignment::value);

    iterator begin() noexcept { return begin_; }
    const_iterator begin() const noexcept { return begin_; }
    iterator end() noexcept { return end_; }
    const_iterator end() const noexcept { return end_; }

    void clear() noexcept { DestructAtEnd(begin_); }

    size_type size() const { return static_cast<size_type>(end_ - begin_); }
    bool empty() const { return end_ == begin_; }
    size_type capacity() const { return static_cast<size_type>(EndCap() - first_); }
    size_type FrontSpare() const { return static_cast<size_type>(begin_ - first_); }
    size_type BackSpare() const { return static_cast<size_type>(EndCap() - end_); }

    reference front() { return *begin_; }
    const_reference front() const { return *begin_; }
    reference back() { return *(end_ - 1); }
    const_reference back() const { return *(end_ - 1); }

    void reserve(size_type n);
    void shrinkToFit() noexcept;
    void pushFront(const_reference x);
    void pushBack(const_reference x);
    void pushFront(value_type&& x);
    void pushBack(value_type&& x);

    template <typename... Args>
    void emplaceBack(Args&&... args);

    void popFront() { DestructAtBegin(begin_ + 1); }
    void popBack() { DestructAtEnd(end_ - 1); }

    void ConstructAtEnd(size_type n);
    void ConstructAtEnd(size_type n, const_reference x);

    template <typename InputIterator>
    typename std::enable_if_t<std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
            && !std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value,
            void> ConstructAtEnd(InputIterator first, InputIterator last);

    template <typename ForwardIterator>
    typename std::enable_if_t<std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value,
            void> ConstructAtEnd(ForwardIterator first, ForwardIterator last);

    void DestructAtBegin(pointer newBegin) {
        DestructAtBegin(newBegin, std::is_trivially_destructible<value_type>());
    }
    void DestructAtBegin(pointer newBegin, std::false_type);
    void DestructAtBegin(pointer newBegin, std::true_type);

    void DestructAtEnd(pointer newLast) noexcept {
        DestructAtEnd(newLast, std::false_type());
    }
    void DestructAtEnd(pointer newLast, std::false_type) noexcept;
    void DestructAtEnd(pointer newLast, std::true_type) noexcept;

    bool Invariants() const;

private:
    void MoveAssignAlloc(SplitBuffer& c, std::true_type) noexcept(std::is_nothrow_move_assignable_v<allocator_type>) {
        Alloc() = std::move(c.Alloc());
    }

    void MoveAssignAlloc(SplitBuffer& c, std::false_type) noexcept {}

    struct ConstructTransaction {
        explicit ConstructTransaction(pointer* p, size_type n) noexcept : pos_(*p), end_(*p + n), dest_(p) {}
        ~ConstructTransaction() { *dest_ = pos_; }
        pointer pos_;
        const pointer end_;
    private:
        pointer *dest_;
    };
};

template <typename T, typename Allocator>
bool SplitBuffer<T, Allocator>::Invariants() const {
    if (first_ == nullptr) {
        if (begin_ != nullptr) return false;
        if (end_ != nullptr) return false;
        if (EndCap() != nullptr) return false;
    } else {
        if (begin_ < first_) return false;
        if (end_ < begin_) return false;
        if (EndCap() < end_) return false;
    }
    return true;
}

template <typename T, typename Allocator>
void SplitBuffer<T, Allocator>::ConstructAtEnd(size_type n) {
    ConstructTransaction tx (&this->end_, n);
    for (; tx.pos_ != tx.end_; ++tx.pos_) {
        AllocTraits::construct(this->Alloc(), std::to_address(tx.pos_));
    }
}

template <typename T, typename Allocator>
void SplitBuffer<T, Allocator>::ConstructAtEnd(size_type n, const_reference x) {
    ConstructTransaction tx (&this->end_, n);
    for (; tx.pos_ != tx.end_; ++tx.pos_) {
        AllocTraits::construct(this->Alloc(), std::to_address(tx.pos_), x);
    }
}

template <typename T, typename Allocator>
template <typename InputIterator>
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
                          && !std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value,
        void> SplitBuffer<T, Allocator>::ConstructAtEnd(InputIterator first, InputIterator last) {
    AllocRR& a = this->Alloc();
    for (; first != last; ++first) {
        if (end_ == EndCap()) {
            size_type oldCap = EndCap() - first_;
            size_type newCap = std::max<size_type>(2 * oldCap, 8);
            SplitBuffer buf(newCap, 0, a);
            for (pointer p = begin_; p != end_; ++p, ++buf.end_)
                AllocTraits::construct(buf.Alloc(), std::to_address(buf.end_), std::move(*p));
            swap(buf);
        }
        AllocTraits::construct(a, std::to_address(this->end_), *first);
        ++this->end_;
    }
}

template <typename T, typename Allocator>
template <typename ForwardIterator>
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value,
        void> SplitBuffer<T, Allocator>::ConstructAtEnd(ForwardIterator first, ForwardIterator last) {
    ConstructTransaction tx (&this->end_, std::distance(first, last));
    for (; tx.pos_ != tx.end_; ++tx.pos_, ++first) {
        AllocTraits::construct(this->Alloc(), std::to_address(tx.pos_), *first);
    }
}

template <typename T, typename Allocator>
inline void SplitBuffer<T, Allocator>::DestructAtBegin(pointer newBegin, std::false_type) {
    while (begin_ != newBegin) {
        AllocTraits::destroy(Alloc(), std::to_address(begin_++));
    }
}

template <typename T, typename Allocator>
inline void SplitBuffer<T, Allocator>::DestructAtBegin(pointer newBegin, std::true_type) {
    begin_ = newBegin;
}

template <typename T, typename Allocator>
inline void SplitBuffer<T, Allocator>::DestructAtEnd(pointer newLast, std::false_type) noexcept {
    while (newLast != end_) {
        AllocTraits::destroy(Alloc(), std::to_address(--end_));
    }
}

template <typename T, typename Allocator>
inline void SplitBuffer<T, Allocator>::DestructAtEnd(pointer newLast, std::true_type) noexcept {
    end_ = newLast;
}

template <typename T, typename Allocator>
SplitBuffer<T, Allocator>::SplitBuffer(size_type cap, size_type start, AllocRR& a) : end_cap_ (nullptr, a) {
    first_ = cap != 0 ? AllocTraits::allocate(Alloc(), cap) : nullptr;
    begin_ = end_ = first_ + start;
    EndCap() = first_ + cap;
}

template <typename T, typename Allocator>
SplitBuffer<T, Allocator>::SplitBuffer() noexcept(std::is_nothrow_default_constructible_v<allocator_type>)
: first_(nullptr), begin_(nullptr), end_(nullptr), end_cap_(nullptr, Allocator()) {}

template <typename T, typename Allocator>
SplitBuffer<T, Allocator>::SplitBuffer(AllocRR& a)
        : first_(nullptr), begin_(nullptr), end_(nullptr), end_cap_(nullptr, a) {}

template <typename T, typename Allocator>
SplitBuffer<T, Allocator>::SplitBuffer(const AllocRR& a)
        : first_(nullptr), begin_(nullptr), end_(nullptr), end_cap_(nullptr, a) {}

template <typename T, typename Allocator>
SplitBuffer<T, Allocator>::~SplitBuffer() {
    clear();
    if (first_) {
        AllocTraits::deallocate(Alloc(), first_, capacity());
    }
}

template <typename T, typename Allocator>
SplitBuffer<T, Allocator>::SplitBuffer(SplitBuffer&& c) noexcept (std::is_nothrow_move_constructible_v<allocator_type>)
: first_(std::move(c.first_)), begin_(std::move(c.begin_)), end_(std::move(c.end_)), end_cap_(std::move(c.end_cap_)) {
    c.first_ = nullptr;
    c.begin_ = nullptr;
    c.end_ = nullptr;
    c.EndCap() = nullptr;
}

template <typename T, typename Allocator>
SplitBuffer<T, Allocator>::SplitBuffer(SplitBuffer&& c, const AllocRR& a) : end_cap_(nullptr, a) {
    if (a == c.Alloc()) {
        first_ = c.first_;
        begin_ = c.begin_;
        end_ = c.end_;
        EndCap() = c.EndCap();
        c.first_ = nullptr;
        c.begin_ = nullptr;
        c.end_ = nullptr;
        c.EndCap() = nullptr;
    } else {
        size_type cap = c.size();
        first_ = AllocTraits::allocate(Alloc(), cap);
        begin_ = end_ = first_;
        EndCap() = first_ + cap;
        ConstructAtEnd(std::move_iterator<iterator>(c.begin()), std::move_iterator<iterator>(c.end()));
    }
}

template <typename T, typename Allocator>
SplitBuffer<T, Allocator>& SplitBuffer<T, Allocator>::operator=(SplitBuffer&& c)
        noexcept ((AllocTraits::propagate_on_container_move_assignment::value && std::is_nothrow_move_assignable_v<allocator_type>)
        || !AllocTraits::propagate_on_container_move_assignment::value) {
    clear();
    shrinkToFit();
    first_ = c.first_;
    begin_ = c.begin_;
    end_ = c.end_;
    EndCap() = c.EndCap();
    MoveAssignAlloc(c, std::integral_constant<bool, AllocTraits::propagate_on_container_move_assignment::value>());
    c.first_ = c.begin_ = c.end_ = c.EndCap() = nullptr;
    return *this;
}

template <typename T, typename Allocator>
void SplitBuffer<T, Allocator>::reserve(size_type n) {
    if (n < capacity()) {
        SplitBuffer<value_type, AllocRR&> t(n, 0, Alloc());
        t.ConstructAtEnd(std::move_iterator<pointer>(begin_), std::move_iterator<pointer>(end_));
        std::swap(first_, t.first_);
        std::swap(begin_, t.begin_);
        std::swap(end_, t.end_);
        std::swap(EndCap(), t.EndCap());
    }
}

template <typename T, typename Allocator>
void SplitBuffer<T, Allocator>::shrinkToFit() noexcept {
    if (capacity() > size()) {
        try {
            SplitBuffer<value_type, AllocRR&> t(size(), 0, Alloc());
            t.ConstructAtEnd(std::move_iterator<pointer>(begin_), std::move_iterator<pointer>(end_));
            t.end_ = t.begin_ + (end_ - begin_);
            std::swap(first_, t.first_);
            std::swap(begin_, t.begin_);
            std::swap(end_, t.end_);
            std::swap(EndCap(), t.EndCap());
        } catch (...) {
        }
    }
}

template <typename T, typename Allocator>
void SplitBuffer<T, Allocator>::pushFront(const_reference x) {
    if (begin_ == first_) {
        if (end_ < EndCap()) {
            difference_type d = EndCap() - end_;
            d = (d + 1) / 2;
            begin_ = std::move_backward(begin_, end_, end_ + d);
            end_ += d;
        } else {
            size_type c = std::max<size_type>(2 * static_cast<size_t>(EndCap() - first_), 1);
            SplitBuffer<value_type, AllocRR&> t(c, (c + 3) / 4, Alloc());
            t.ConstructAtEnd(std::move_iterator<pointer>(begin_), std::move_iterator<pointer>(end_));
            std::swap(first_, t.first_);
            std::swap(begin_, t.begin_);
            std::swap(end_, t.end_);
            std::swap(EndCap(), t.EndCap());
        }
    }
    AllocTraits::construct(Alloc(), std::to_address(begin_ - 1), x);
    --begin_;
}

template <typename T, typename Allocator>
void SplitBuffer<T, Allocator>::pushFront(value_type&& x) {
    if (begin_ == first_) {
        if (end_ < EndCap()) {
            difference_type d = EndCap() - end_;
            d = (d + 1) / 2;
            begin_ = std::move_backward(begin_, end_, end_ + d);
            end_ += d;
        } else {
            size_type c = std::max<size_type>(2 * static_cast<size_t>(EndCap() - first_), 1);
            SplitBuffer<value_type, AllocRR&> t(c, (c + 3) / 4, Alloc());
            t.ConstructAtEnd(std::move_iterator<pointer>(begin_), std::move_iterator<pointer>(end_));
            std::swap(first_, t.first_);
            std::swap(begin_, t.begin_);
            std::swap(end_, t.end_);
            std::swap(EndCap(), t.EndCap());
        }
    }
    AllocTraits::construct(Alloc(), std::to_address(begin_ - 1), std::move(x));
    --begin_;
}

template <typename T, typename Allocator>
inline void SplitBuffer<T, Allocator>::pushBack(const_reference x) {
    if (end_ == EndCap()) {
        if (begin_ > first_) {
            difference_type d = begin_ - first_;
            d = (d + 1) / 2;
            end_ = std::move(begin_, end_, begin_ - d);
            begin_ -= d;
        } else {
            size_type c = std::max<size_type>(2 * static_cast<size_t>(EndCap() - first_), 1);
            SplitBuffer<value_type, AllocRR&> t(c, c / 4, Alloc());
            t.ConstructAtEnd(std::move_iterator<pointer>(begin_), std::move_iterator<pointer>(end_));
            std::swap(first_, t.first_);
            std::swap(begin_, t.begin_);
            std::swap(end_, t.end_);
            std::swap(EndCap(), t.EndCap());
        }
    }
    AllocTraits::construct(Alloc(), std::to_address(end_), x);
    ++end_;
}

template <typename T, typename Allocator>
inline void SplitBuffer<T, Allocator>::pushBack(value_type&& x) {
    if (end_ == EndCap()) {
        if (begin_ > first_) {
            difference_type d = begin_ - first_;
            d = (d + 1) / 2;
            end_ = std::move(begin_, end_, begin_ - d);
            begin_ -= d;
        } else {
            size_type c = std::max<size_type>(2 * static_cast<size_t>(EndCap() - first_), 1);
            SplitBuffer<value_type, AllocRR&> t(c, c / 4, Alloc());
            t.ConstructAtEnd(std::move_iterator<pointer>(begin_), std::move_iterator<pointer>(end_));
            std::swap(first_, t.first_);
            std::swap(begin_, t.begin_);
            std::swap(end_, t.end_);
            std::swap(EndCap(), t.EndCap());
        }
    }
    AllocTraits::construct(Alloc(), std::to_address(end_), std::move(x));
    ++end_;
}

template <typename T, typename Allocator>
template <typename... Args>
void SplitBuffer<T, Allocator>::emplaceBack(Args&&... args) {
    if (end_ == EndCap()) {
        if (begin_ > first_) {
            difference_type d = begin_ - first_;
            d = (d + 1) / 2;
            end_ = std::move(begin_, end_, begin_ - d);
            begin_ -= d;
        } else {
            size_type c = std::max<size_type>(2 * static_cast<size_t>(EndCap() - first_), 1);
            SplitBuffer<value_type, AllocRR&> t(c, c / 4, Alloc());
            t.ConstructAtEnd(std::move_iterator<pointer>(begin_), std::move_iterator<pointer>(end_));
            std::swap(first_, t.first_);
            std::swap(begin_, t.begin_);
            std::swap(end_, t.end_);
            std::swap(EndCap(), t.EndCap());
        }
    }
    AllocTraits::construct(Alloc(), std::to_address(end_), std::forward<Args>(args)...);
    ++end_;
}

template <typename T, typename Allocator>
class Vector : private VectorBase<T, Allocator> {
private:
    using Base = VectorBase<T, Allocator>;
    using DefaultAllocatorType = std::allocator<T>;
public:
    using Self = Vector;
    using value_type = T;
    using allocator_type = Allocator;
    using AllocTraits = typename Base::AllocTraits;
    using reference = typename Base::reference;
    using const_reference = typename Base::const_reference;
    using size_type = typename Base::size_type;
    using difference_type = typename Base::difference_type;
    using pointer = typename Base::pointer;
    using const_pointer = typename Base::const_pointer;

    using iterator = WrapIter<pointer>;
    using const_iterator = WrapIter<const_pointer>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>);

    Vector() noexcept (std::is_nothrow_default_constructible_v<allocator_type>) {}
    explicit Vector(const allocator_type& a) noexcept : Base(a) {}
    explicit Vector(size_type n);
    explicit Vector(size_type n, const allocator_type& a);
    Vector(size_type n, const value_type& x);
    Vector(size_type n, const value_type& x, const allocator_type& a);

    template<typename InputIterator>
    Vector(InputIterator first,
           typename std::enable_if_t<std::integral_constant<bool,
                   std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
                                     && std::integral_constant<bool,
                   std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value
                                     &&
                                     std::is_constructible_v<value_type, typename std::iterator_traits<InputIterator>::reference>,
                   InputIterator> last);

    template<typename InputIterator>
    Vector(InputIterator first, InputIterator last, const allocator_type &a,
           typename std::enable_if_t<std::integral_constant<bool,
                   std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
                                     && std::integral_constant<bool,
                   std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value
                                     &&
                                     std::is_constructible_v<value_type, typename std::iterator_traits<InputIterator>::reference>
           >* = 0);

    template<typename ForwardIterator>
    Vector(ForwardIterator first,
           typename std::enable_if_t<std::integral_constant<bool,
                   std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value
                                     &&
                                     std::is_constructible_v<value_type, typename std::iterator_traits<ForwardIterator>::reference>,
                   ForwardIterator> last);

    template<typename ForwardIterator>
    Vector(ForwardIterator first, ForwardIterator last, const allocator_type &a,
           typename std::enable_if_t<std::integral_constant<bool,
                   std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value
                                     &&
                                     std::is_constructible_v<value_type, typename std::iterator_traits<ForwardIterator>::reference>
           >* = 0);

    ~Vector(){
        AnnotateDelete();
    }

    Vector(const Vector& x);
    Vector(const Vector& x, const allocator_type& a);
    Vector& operator=(const Vector& x);

    Vector(std::initializer_list<value_type> il);
    Vector(std::initializer_list<value_type> il, const allocator_type& a);
    Vector(Vector&& x) noexcept;
    Vector(Vector&& x, const allocator_type& a);

    Vector& operator=(Vector&& x) noexcept(std::integral_constant<bool,
            AllocTraits::propagate_on_container_move_assignment::value
            || AllocTraits::is_always_equal::value>::value);

    Vector& operator=(std::initializer_list<value_type> il) {
        assign(il.begin(), il.end());
        return *this;
    }

    template <typename InputIterator>
    typename std::enable_if_t<std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
                              && std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value
                              &&
                              std::is_constructible_v<value_type, typename std::iterator_traits<InputIterator>::reference>,
            void> assign (InputIterator first, InputIterator last);

    template <typename ForwardIterator>
    typename std::enable_if_t<std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value
                              &&
                              std::is_constructible_v<value_type, typename std::iterator_traits<ForwardIterator>::reference>,
            void> assign (ForwardIterator first, ForwardIterator last);

    void assign(size_type n, const_reference u);
    void assign(std::initializer_list<value_type> il) {
        assign(il.begin(), il.end());
    }

    allocator_type getAllocator() const noexcept { return this->alloc(); }

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    size_type size() const noexcept { return static_cast<size_type> (this->end_ - this->begin_); }
    size_type capacity() const noexcept { return Base::capacity(); }
    [[nodiscard]] bool empty() const noexcept { return this->begin_ == this->end_; }
    size_type maxSize() const noexcept;
    void reserve(size_type n);
    void shrinkToFit() noexcept;

    reference operator[](size_type n) noexcept;
    const_reference operator[](size_type n) const noexcept;
    reference at(size_type n);
    const_reference at(size_type n) const;
    reference front() noexcept {
        assert(!empty());
        return *this->begin_;
    }
    const_reference front() const noexcept {
        assert(!empty());
        return *this->begin_;
    }
    reference back() noexcept {
        assert(!empty());
        return *(this->end_ - 1);
    }
    const_reference back() const noexcept {
        assert(!empty());
        return *(this->end_ - 1);
    }
    value_type* data() noexcept { return std::to_address(this->begin_); }
    const value_type* data() const noexcept { return std::to_address(this->begin_); }

    template <typename Arg>
    void EmplaceBack(Arg&& arg) {
        emplaceBack(std::forward<Arg>(arg));
    }

    void pushBack(const_reference x);
    void pushBack(value_type&& x);

    template <typename... Args>
    reference emplaceBack(Args&&... args);

    void popBack();

    iterator insert(const_iterator position, const_reference x);
    iterator insert(const_iterator position, value_type&& x);
    template <typename... Args>
    iterator emplace(const_iterator position, Args&&... args);
    iterator insert(const_iterator position, size_type n, const_reference x);

    template <typename InputIterator>
    typename std::enable_if_t<std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
                              && std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value
                              &&
                              std::is_constructible_v<value_type, typename std::iterator_traits<InputIterator>::reference>,
            iterator> insert(const_iterator position, InputIterator first, InputIterator last);

    template <typename ForwardIterator>
    typename std::enable_if_t<std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value
                              &&
                              std::is_constructible_v<value_type, typename std::iterator_traits<ForwardIterator>::reference>,
            iterator> insert(const_iterator position, ForwardIterator first, ForwardIterator last);

    iterator insert(const_iterator position, std::initializer_list<value_type> il) {
        return insert(position, il.begin(), il.end());
    }

    iterator erase(const_iterator position);
    iterator erase(const_iterator first, const_iterator last);

    void clear() noexcept {
        size_type OldSize = size();
        Base::clear();
        AnnotateShrink(OldSize);
        InvalidateAllIterators();
    }

    void resize(size_type sz);
    void resize(size_type sz, const_reference x);

    bool Invariants() const;

private:
    void InvalidateAllIterators();
    void InvalidateAllIterators(pointer newLast);
    void Vallocate(size_type n);
    void Vdeallocate() noexcept;
    size_type Recommend(size_type newSize) const;
    void ConstructAtEnd(size_type n);
    void ConstructAtEnd(size_type n, const_reference x);

    template <typename ForwardIterator>
    typename std::enable_if_t<std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value,
            void> ConstructAtEnd(ForwardIterator first, ForwardIterator last, size_type n);

    void Append(size_type n);
    void Append(size_type n, const_reference x);

    iterator MakeIter(pointer p) noexcept;
    const_iterator MakeIter(const_pointer p) const noexcept;
    void SwapOutCircularBuffer(SplitBuffer<value_type, allocator_type&>& v);
    pointer SwapOutCircularBuffer(SplitBuffer<value_type, allocator_type&>& v, pointer p);

    void MoveRange(pointer fromS, pointer fromE, pointer to);
    void MoveAssign(Vector& c, std::true_type) noexcept(std::is_nothrow_move_assignable_v<allocator_type>);
    void MoveAssign(Vector& c, std::false_type) noexcept(AllocTraits::is_always_equal::value);

    void DestructAtEnd(pointer newLast) noexcept {
        InvalidateIteratorsPast(newLast);
        size_type oldSize = size();
        Base::DestructAtEnd(newLast);
        AnnotateShrink(oldSize);
    }

    template <typename U>
    inline void PushBackSlowPath(U&& x);

    template <typename... Args>
    inline void EmplaceBackSlowPath(Args&&... args);

    void AnnotateContiguousContainer(const void*, const void*, const void*, const void*) const noexcept {}

    void AnnotateNew(size_type currentSize) const noexcept {
        AnnotateContiguousContainer(data(), data() + capacity(), data() + capacity(), data() + currentSize);
    }

    void AnnotateDelete() const noexcept {
        AnnotateContiguousContainer(data(), data() + capacity(), data() + size(), data() + capacity());
    }

    void AnnotateIncrease(size_type n) const noexcept {
        AnnotateContiguousContainer(data(), data() + capacity(), data() + size(), data() + size() + n);
    }

    void AnnotateShrink(size_type oldSize) const noexcept {
        AnnotateContiguousContainer(data(), data() + capacity(), data() + oldSize, data() + size());
    }

    void InvalidateIteratorsPast(pointer newLast);

    struct ConstructTransaction {
        explicit ConstructTransaction(Vector& v, size_type n)
                : v_(v), pos_(v.end_), new_end_(v.end_ + n) {
        }
        ~ConstructTransaction() {
            v_.end_ = pos_;
        }

        Vector& v_;
        pointer pos_;
        const_pointer const new_end_;

    private:
        ConstructTransaction(ConstructTransaction const&) = delete;
        ConstructTransaction& operator=(ConstructTransaction const&) = delete;
    };

    template <typename... Args>
    void ConstructOneAtEnd(Args&&... args) {
        ConstructTransaction tx (*this, 1);
        AllocTraits::construct(this->Alloc(), std::to_address(tx.pos_), std::forward<Args>(args)...);
        ++tx.pos_;
    }
};

template<typename InputIterator,
        typename Alloc = typename std::allocator<typename std::iterator_traits<InputIterator>::value_type>,
        typename = void>
Vector(InputIterator, InputIterator) -> Vector<typename std::iterator_traits<InputIterator>::value_type, Alloc>;

template<typename InputIterator,
        typename Alloc,
        typename = void>
Vector(InputIterator, InputIterator, Alloc) -> Vector<typename std::iterator_traits<InputIterator>::value_type, Alloc>;

template <typename T, typename Allocator>
void Vector<T, Allocator>::SwapOutCircularBuffer(SplitBuffer<value_type, allocator_type&>& v) {
    AnnotateDelete();
    AllocTraits::construct_backward_with_exception_guarantees(this->Alloc(), this->begin_, this->end_, v.begin_);
    std::swap(this->begin_, v.begin_);
    std::swap(this->end_, v.end_);
    std::swap(this->EndCap(), v.EndCap());
    v.first_ = v.begin_;
    AnnotateNew(size());
    InvalidateAllIterators();
}

template <typename T, typename Allocator>
typename Vector<T, Allocator>::pointer Vector<T, Allocator>::SwapOutCircularBuffer(SplitBuffer<value_type, allocator_type&>& v, pointer p) {
    AnnotateDelete();
    pointer r = v.begin_;
    static_assert(std::integral_constant<bool,
            std::is_same_v<decltype(std::declval<Allocator>.construct(std::declval<typename Allocator::value_type*>(),
                    std::declval<typename Allocator::value_type&&>())), std::true_type>>::value && !std::allocator<Allocator>::value
                    || std::is_move_constructible_v<typename Allocator::value_type>);
    pointer b1 = this->begin_;
    pointer e1 = p;
    pointer e2 = v.begin_;
    while (e1 != b1) {
        AllocTraits::construct(this->Alloc(), std::to_address(e2 - 1), std::move_if_noexcept(*--e1));
        --e2;
    }
    b1 = p;
    e1 = this->end_;
    e2 = v.end_;
    while (e1 != b1) {
        AllocTraits::construct(this->Alloc(), std::to_address(e2 - 1), std::move_if_noexcept(*--e1));
        --e2;
    }
    std::swap(this->begin_, v.begin_);
    std::swap(this->end_, v.end_);
    std::swap(this->EndCap(), v.EndCap());
    v.first_ = v.begin_;
    AnnotateNew(size());
    InvalidateAllIterators();
    return r;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Vallocate(size_type n) {
    if (n > maxSize()) {
        throw std::length_error("");
    }
    this->begin_ = this->end_ = AllocTraits(this->Alloc(), n);
    this->EndCap() = this->begin_ + n;
    AnnotateNew(0);
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Vdeallocate() noexcept {
    if (this->begin_ != nullptr) {
        clear();
        AllocTraits::deallocate(this->Alloc(), this->begin_, capacity());
        this->begin_ = this->end_ = this->EndCap() = nullptr;
    }
}

template <typename T, typename Allocator>
typename Vector<T, Allocator>::size_type Vector<T, Allocator>::maxSize() const noexcept {
    return std::min<size_type>(AllocTraits::maxSize(this->Alloc()), std::numeric_limits<difference_type>::max());
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::size_type Vector<T, Allocator>::Recommend(size_type newSize) const {
    const size_type ms = maxSize();
    if (newSize > ms) {
        throw std::length_error("");
    }
    const size_type cap = capacity();
    if (cap >= ms / 2) {
        return ms;
    }
    return std::max<size_type>(2 * cap, newSize);
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::ConstructAtEnd(size_type n) {
    ConstructTransaction tx (*this, n);
    for (; tx.pos_ != tx.new_end_; ++tx.pos_) {
        AllocTraits::construct(this->Alloc(), std::to_address(tx.pos_));
    }
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::ConstructAtEnd(size_type n, const_reference x) {
    ConstructTransaction tx (*this, n);
    for (; tx.pos_ != tx.new_end_; ++tx.pos_) {
        AllocTraits::construct(this->Alloc(), std::to_address(tx.pos_), x);
    }
}

template <typename T, typename Allocator>
template <typename ForwardIterator>
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value,
        void> Vector<T, Allocator>::ConstructAtEnd(ForwardIterator first, ForwardIterator last, size_type n) {
    ConstructTransaction tx (*this, n);
    for (; first != last; ++first, (void) ++tx.pos_) {
        AllocTraits::construct(this->Alloc(), std::to_address(n), *first);
    }
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Append(size_type n) {
    if (static_cast<size_type>(this->EndCap() - this->end_) >= n) {
        this->ConstructAtEnd(n);
    } else {
        allocator_type& a = this->Alloc();
        SplitBuffer<value_type, allocator_type&> v(Recommend(size() + n), size(), a);
        v.ConstructAtEnd(n);
        SwapOutCircularBuffer(v);
    }
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Append(size_type n, const_reference x) {
    if (static_cast<size_type>(this->EndCap() - this->end_) >= n) {
        this->ConstructAtEnd(n, x);
    } else {
        allocator_type& a = this->Alloc();
        SplitBuffer<value_type, allocator_type&> v(Recommend(size() + n), size(), a);
        v.ConstructAtEnd(n, x);
        SwapOutCircularBuffer(v);
    }
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_type n) {
    if (n > 0) {
        Vallocate(n);
        ConstructAtEnd(n);
    }
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_type n, const allocator_type& a) : Base(a) {
    if (n > 0) {
        Vallocate(n);
        ConstructAtEnd(n);
    }
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_type n, const value_type& x) {
    if (n > 0) {
        Vallocate(n);
        ConstructAtEnd(n, x);
    }
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_type n, const value_type& x, const allocator_type& a) : Base(a) {
    if (n > 0) {
        Vallocate(n);
        ConstructAtEnd(n, x);
    }
}

template <typename T, typename Allocator>
template <typename InputIterator>
Vector<T, Allocator>::Vector(InputIterator first,
       typename std::enable_if_t<std::integral_constant<bool,
               std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
                                 && std::integral_constant<bool,
               std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value
                                 &&
                                 std::is_constructible_v<value_type, typename std::iterator_traits<InputIterator>::reference>,
               InputIterator> last) {
    for (; first != last; ++first) {
        EmplaceBack(*first);
    }
}

template <typename T, typename Allocator>
template <typename InputIterator>
Vector<T, Allocator>::Vector(InputIterator first, InputIterator last, const allocator_type &a,
       typename std::enable_if_t<std::integral_constant<bool,
               std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
                                 && std::integral_constant<bool,
               std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value
                                 &&
                                 std::is_constructible_v<value_type, typename std::iterator_traits<InputIterator>::reference>
       >*) : Base(a) {
    for (; first != last; ++first) {
        EmplaceBack(*first);
    }
}

template <typename T, typename Allocator>
template<typename ForwardIterator>
Vector<T, Allocator>::Vector(ForwardIterator first,
       typename std::enable_if_t<std::integral_constant<bool,
               std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value
                                 &&
                                 std::is_constructible_v<value_type, typename std::iterator_traits<ForwardIterator>::reference>,
               ForwardIterator> last) {
    size_type n = static_cast<size_type>(std::distance(first, last));
    if (n > 0) {
        Vallocate(n);
        ConstructAtEnd(first, last, n);
    }
}

template <typename T, typename Allocator>
template<typename ForwardIterator>
Vector<T, Allocator>::Vector(ForwardIterator first, ForwardIterator last, const allocator_type &a,
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value
                          &&
                          std::is_constructible_v<value_type, typename std::iterator_traits<ForwardIterator>::reference>
>*) : Base(a) {
    size_type n = static_cast<size_type>(std::distance(first, last));
    if (n > 0) {
        Vallocate(n);
        ConstructAtEnd(first, last, n);
    }
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector& x) : Base(AllocTraits::select_on_container_copy_construction(x.Alloc())) {
    size_type n = x.size();
    if (n > 0) {
        Vallocate(n);
        ConstructAtEnd(x.begin_, x.end_, n);
    }
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector& x, const allocator_type& a) : Base(a) {
    size_type n = x.size();
    if (n > 0) {
        Vallocate(n);
        ConstructAtEnd(x.begin_, x.end_, n);
    }
}

template <typename T, typename Allocator>
inline Vector<T, Allocator>::Vector(Vector&& x) noexcept : Base(std::move(x.Alloc())) {
    this->begin_ = x.begin_;
    this->end_ = x.end_;
    this->EndCap() = x.EndCap();
    x.begin_ = x.end_ = x.EndCap() = nullptr;
}

template <typename T, typename Allocator>
inline Vector<T, Allocator>::Vector(Vector&& x, const allocator_type& a) : Base(a) {
    if (a == x.Alloc()) {
        this->begin_ = x.begin_;
        this->end_ = x.end_;
        this->EndCap() = x.EndCap();
        x.begin_ = x.end_ = x.EndCap() = nullptr;
    } else {
        assign(std::move_iterator<iterator>(x.begin()), std::move_iterator<iterator>(x.end()));
    }
}

template <typename T, typename Allocator>
inline Vector<T, Allocator>::Vector(std::initializer_list<value_type> il) {
    if (il.size() > 0) {
        Vallocate(il.size());
        ConstructAtEnd(il.begin(), il.end(), il.size());
    }
}

template <typename T, typename Allocator>
inline Vector<T, Allocator>::Vector(std::initializer_list<value_type> il, const allocator_type& a) : Base(a) {
    if (il.size() > 0) {
        Vallocate(il.size());
        ConstructAtEnd(il.begin(), il.end(), il.size());
    }
}

template <typename T, typename Allocator>
inline Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& x) noexcept(std::integral_constant<bool,
        AllocTraits::propagate_on_container_move_assignment::value
        || AllocTraits::is_always_equal::value>::value) {
    MoveAssign(x, std::integral_constant<bool, AllocTraits::propagate_on_container_move_assignment::value>());
    return *this;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::MoveAssign(Vector& c, std::false_type) noexcept(AllocTraits::is_always_equal::value) {
    if (Base::Alloc() != c.Alloc()) {
        assign(std::move_iterator<iterator>(c.begin()), std::move_iterator<iterator>(c.end()));
    } else {
        MoveAssign(c, std::true_type());
    }
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::MoveAssign(Vector& c, std::true_type) noexcept(std::is_nothrow_move_assignable_v<allocator_type>) {
    Vdeallocate();
    Base::MoveAssignAlloc(c);
    this->begin_ = c.begin_;
    this->end_ = c.end_;
    this->EndCap() = c.EndCap();
    c.begin_ = c.end_ = c.EndCap() = nullptr;
}

template <typename T, typename Allocator>
inline Vector<T, Allocator>& Vector<T, Allocator>::operator=(const Vector& x) {
    if (this != &x) {
        Base::CopyAssignAlloc(x);
        assign(x.begin_, x.end_);
    }
    return *this;
}

template <typename T, typename Allocator>
template <typename InputIterator>
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
                          && std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value
                          &&
                          std::is_constructible_v<typename Vector<T, Allocator>::value_type, typename std::iterator_traits<InputIterator>::reference>,
        void> Vector<T, Allocator>::assign(InputIterator first, InputIterator last) {
    clear();
    for (; first != +last; ++first) {
        EmplaceBack(*first);
    }
}

template <typename T, typename Allocator>
template <typename ForwardIterator>
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value
                          &&
                          std::is_constructible_v<typename Vector<T, Allocator>::value_type, typename std::iterator_traits<ForwardIterator>::reference>,
        void> Vector<T, Allocator>::assign (ForwardIterator first, ForwardIterator last) {
    size_type newSize = static_cast<size_type>(std::distance(first, last));
    if (newSize <= capacity()) {
        ForwardIterator mid = last;
        bool growing = false;
        if (newSize > size()) {
            growing = true;
            mid = first;
            std::advance(mid, size());
        }
        pointer m = std::copy(first, mid, this->begin_);
        if (growing) {
            ConstructAtEnd(mid, last, newSize - size());
        } else {
            this->DestructAtEnd(m);
        }
    } else {
        Vdeallocate();
        Vallocate(Recommend(newSize));
        ConstructAtEnd(first, last, newSize);
    }
    InvalidateAllIterators();
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::assign(size_type n, const_reference u) {
    if (n <= capacity()) {
        size_type s = size();
        std::fill_n(this->begin_, std::min(n, s), u);
        if (n > s) {
            ConstructAtEnd(n - s, u);
        } else {
            this->DestructAtEnd(this->begin_ + n);
        }
    } else {
        Vdeallocate();
        Vallocate(Recommend(static_cast<size_type>(n)));
        ConstructAtEnd(n, u);
    }
    InvalidateAllIterators();
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::iterator Vector<T, Allocator>::MakeIter(pointer p) noexcept {
    return iterator(p);
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::MakeIter(const_pointer p) const noexcept {
    return const_iterator(p);
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::iterator Vector<T, Allocator>::begin() noexcept {
    return MakeIter(this->begin_);
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::begin() const noexcept {
    return MakeIter(this->begin_);
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::iterator Vector<T, Allocator>::end() noexcept {
    return MakeIter(this->end_);
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::end() const noexcept {
    return MakeIter(this->end_);
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::reference Vector<T, Allocator>::operator[](size_type n) noexcept {
    assert(n < size());
    return this->begin_[n];
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::operator[](size_type n) const noexcept {
    assert(n < size());
    return this->begin_[n];
}

template <typename T, typename Allocator>
typename Vector<T, Allocator>::reference Vector<T, Allocator>::at(size_type n) {
    if (n >= size()) {
        throw std::out_of_range("");
    }
    return this->begin_[n];
}

template <typename T, typename Allocator>
typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::at(size_type n) const {
    if (n >= size()) {
        throw std::out_of_range("");
    }
    return this->begin_[n];
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::reserve(size_type n) {
    if (n > capacity()) {
        allocator_type& a = this->Alloc();
        SplitBuffer<value_type, allocator_type&> v(n, size(), a);
        SwapOutCircularBuffer(v);
    }
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::shrinkToFit() noexcept {
    if (capacity() > size()) {
        try {
            allocator_type &a = this->Alloc();
            SplitBuffer<value_type, allocator_type &> v(size(), size(), a);
            SwapOutCircularBuffer(v);
        } catch (...) {
        }
    }
}

template <typename T, typename Allocator>
template <typename U>
void Vector<T, Allocator>::PushBackSlowPath(U&& x) {
    allocator_type &a = this->Alloc();
    SplitBuffer<value_type, allocator_type &> v(Recommend(size() + 1), size(), a);
    AllocTraits::construct(a, std::to_address(v.end_), std::forward<U>(x));
    v.end_++;
    SwapOutCircularBuffer(v);
}

template <typename T, typename Allocator>
inline void Vector<T, Allocator>::pushBack(const_reference x) {
    if (this->end_ != this->EndCap()) {
        ConstructOneAtEnd(x);
    } else {
        PushBackSlowPath(x);
    }
}

template <typename T, typename Allocator>
inline void Vector<T, Allocator>::pushBack(value_type&& x) {
    if (this->end_ != this->EndCap()) {
        ConstructOneAtEnd(std::move(x));
    } else {
        PushBackSlowPath(std::move(x));
    }
}

template <typename T, typename Allocator>
template <typename... Args>
void Vector<T, Allocator>::EmplaceBackSlowPath(Args&&... args) {
    allocator_type &a = this->Alloc();
    SplitBuffer<value_type, allocator_type &> v(Recommend(size() + 1), size(), a);
    AllocTraits::construct(a, std::to_address(v.end_), std::forward<Args>(args)...);
    v.end_++;
    SwapOutCircularBuffer(v);
}

template <typename T, typename Allocator>
template <typename... Args>
inline typename Vector<T, Allocator>::reference Vector<T, Allocator>::emplaceBack(Args&&... args) {
    if (this->end_ != this->EndCap()) {
        ConstructOneAtEnd(std::forward<Args>(args)...);
    } else {
        EmplaceBackSlowPath(std::forward<Args>(args)...);
    }
    return this->back();
}

template <typename T, typename Allocator>
inline void Vector<T, Allocator>::popBack() {
    assert(!empty());
    this->DestructAtEnd(this->end_ - 1);
}

template <typename T, typename Allocator>
inline typename Vector<T, Allocator>::iterator Vector<T, Allocator>::erase(const_iterator position) {
    assert(position != end());
    difference_type ps = position - cbegin();
    pointer p = this->begin_ + ps;
    this->DestructAtEnd(std::move(p + 1, this->end_, p));
    this->InvalidateIteratorsPast(p - 1);
    iterator r = MakeIter(p);
    return r;
}

template <typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::erase(const_iterator first, const_iterator last) {
    assert(first <= last);
    pointer p = this->begin_ + (first - begin());
    if (first != last) {
        this->DestructAtEnd(std::move(p + (last - first), this->end_, p));
        this->InvalidateIteratorsPast(p - 1);
    }
    iterator r = MakeIter(p);
    return r;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::MoveRange(pointer fromS, pointer fromE, pointer to) {
    pointer oldLast = this->end_;
    difference_type n = oldLast - to;
    {
        pointer i = fromS + n;
        ConstructTransaction tx(*this, fromE - i);
        for (; i < fromE; ++i, ++tx.pos_) {
            AllocTraits::construct(this->Alloc(), std::to_address(tx.pos_), std::move(*i));
        }
    }
    std::move_backward(fromS, fromS + n, oldLast);
}

template <typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::insert(const_iterator position, const_reference x) {
    pointer p = this->begin_ + (position - begin());
    if (this->end_ < this->EndCap()) {
        if (p == this->end_) {
            ConstructOneAtEnd(x);
        } else {
            MoveRange(p, this->end_, p + 1);
            const_pointer xr = std::pointer_traits<const_pointer>::pointer_to(x);
            if (p <= xr && xr < this->end) {
                ++xr;
            }
            *p = *xr;
        }
    } else {
        allocator_type& a = this->Alloc();
        SplitBuffer<value_type, allocator_type&> v(Recommend(size() + 1), p - this->begin_, a);
        v.pushBack(x);
        p = SwapOutCircularBuffer(v, p);
    }
    return MakeIter(p);
}

template <typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::insert(const_iterator position, value_type&& x) {
    pointer p = this->begin_ + (position - begin());
    if (this->end_ < this->EndCap()) {
        if (p == this->end_) {
            ConstructOneAtEnd(std::move(x));
        } else {
            MoveRange(p, this->end_, p + 1);
            *p = std::move(x);
        }
    } else {
        allocator_type& a = this->Alloc();
        SplitBuffer<value_type, allocator_type&> v(Recommend(size() + 1), p - this->begin_, a);
        v.pushBack(std::move(x));
        p = SwapOutCircularBuffer(v, p);
    }
    return MakeIter(p);
}

template <typename T, typename Allocator>
template <typename... Args>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::emplace(const_iterator position, Args&&... args) {
    pointer p = this->begin_ + (position - begin());
    if (this->end_ < this->EndCap()) {
        if (p == this->end_) {
            ConstructOneAtEnd(std::forward<Args>(args)...);
        } else {
            typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type v;
            AllocTraits::construct(this->Alloc(), reinterpret_cast<value_type *>(std::addressof(v)), std::forward<Args>(args)...);
            MoveRange(p, this->end_, p + 1);
            *p = std::move(v);
        }
    } else {
        allocator_type& a = this->Alloc();
        SplitBuffer<value_type, allocator_type&> v(Recommend(size() + 1), p - this->begin_, a);
        v.emplaceBack(std::forward<Args>(args)...);
        p = SwapOutCircularBuffer(v, p);
    }
    return MakeIter(p);
}

template <typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::insert(const_iterator position, size_type n, const_reference x) {
    pointer p = this->begin_ + (position - begin());
    if (n > 0) {
        if (n <= static_cast<size_type>(this->EndCap() - this->end_)) {
            size_type oldN = n;
            pointer oldLast = this->end_;
            if (n > static_cast<size_type>(this->end_ - p)) {
                size_type cx = n - (this->end_ - p);
                ConstructOneAtEnd(cx, x);
                n -= cx;
            }
            if (n > 0) {
                MoveRange(p, oldLast, p + oldN);
                const_pointer xr = std::pointer_traits<const_pointer>::pointer_to(x);
                if (p <= xr && xr < this->end_) {
                    xr += oldN;
                }
                std::fill_n(p, n, *xr);
            }
        } else {
            allocator_type& a = this->Alloc();
            SplitBuffer<value_type, allocator_type&> v(Recommend(size() + n), p - this->begin_, a);
            v.ConstructAtEnd(n, x);
            p = SwapOutCircularBuffer(v, p);
        }
    }
    return MakeIter(p);
}

template <typename T, typename Allocator>
template <typename InputIterator>
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::input_iterator_tag>>::value
                          && std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category, std::forward_iterator_tag>>::value
                          &&
                          std::is_constructible_v<typename Vector<T, Allocator>::value_type, typename std::iterator_traits<InputIterator>::reference>,
        typename Vector<T, Allocator>::iterator>
Vector<T, Allocator>::insert(const_iterator position, InputIterator first, InputIterator last) {
    difference_type off = position - begin();
    pointer p = this->begin_ + off;
    allocator_type& a = this->Alloc();
    pointer oldLast = this->end_;
    for (; this->end_ != this->EndCap() && first != last; ++first) {
        ConstructAtEnd(*first);
    }
    SplitBuffer<value_type, allocator_type&> v(a);
    if (first != last) {
        try {
            v.ConstructAtEnd(first, last);
            difference_type oldSize = oldLast - this->begin_;
            difference_type oldP = p - this->begin_;
            reserve(Recommend(size() + v.size()));
            p = this->begin_ + oldP;
            oldLast = this->begin_ + oldSize;
        } catch (...) {
            erase(MakeIter(oldLast), end());
            throw;
        }
    }
    p = std::rotate(p, oldLast, this->end_);
    insert(MakeIter(p), std::make_move_iterator(v.begin()), std::make_move_iterator(v.end()));
    return begin() + off;
}

template <typename T, typename Allocator>
template <typename ForwardIterator>
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<ForwardIterator>::iterator_category, std::forward_iterator_tag>>::value
                          &&
                          std::is_constructible_v<T, typename std::iterator_traits<ForwardIterator>::reference>,
        typename Vector<T, Allocator>::iterator>
Vector<T, Allocator>::insert(const_iterator position, ForwardIterator first, ForwardIterator last) {
    pointer p = this->begin_ + (position - begin());
    difference_type n = std::distance(first, last);
    if (n > 0) {
        if (n <= this->EndCap() - this->end_) {
            size_type oldN = n;
            pointer oldLast = this->end_;
            ForwardIterator m = last;
            difference_type dx = this->end_ - p;
            if (n > dx) {
                m = first;
                difference_type diff = this->end_ - p;
                std::advance(m, diff);
                ConstructAtEnd(m, last, n - diff);
                n = dx;
            }
            if (n > 0) {
                MoveRange(p, oldLast, p + oldN);
                std::copy(first, m, p);
            }
        } else {
            allocator_type& a = this->Alloc();
            SplitBuffer<value_type, allocator_type&> v(Recommend(size() + n), p - this->begin_, a);
            v.ConstructAtEnd(first, last);
            p = SwapOutCircularBuffer(v, p);
        }
        return MakeIter(p);
    }
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::resize(size_type sz) {
    size_type cs = size();
    if (cs < sz) {
        this->Append(sz - cs);
    } else if (cs > sz) {
        this->DestructAtEnd(this->begin + sz);
    }
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::resize(size_type sz, const_reference x) {
    size_type cs = size();
    if (cs < sz) {
        this->Append(sz - cs, x);
    } else if (cs > sz) {
        this->DestructAtEnd(this->begin + sz);
    }
}

template <typename T, typename Allocator>
bool Vector<T, Allocator>::Invariants() const {
    if (this->begin_ == nullptr) {
        if (this->end_ != nullptr || this->EndCap() != nullptr) {
            return false;
        }
    } else {
        if (this->begin_ > this->end_) {
            return false;
        }
        if (this->begin_ == this->EndCap()) {
            return false;
        }
        if (this->end_ > this->EndCap()) {
            return false;
        }
    }
    return true;
}

template <typename T, typename Allocator>
inline void Vector<T, Allocator>::InvalidateAllIterators() {}

template <typename T, typename Allocator>
inline void Vector<T, Allocator>::InvalidateIteratorsPast(pointer newLast) {
}


int main() {

}