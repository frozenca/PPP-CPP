#include <cassert>
#include <limits>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <functional>
#include <iterator>
#include <utility>
#include <initializer_list>

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

template <typename Alloc>
class AllocatorDestructor {
    using AllocTraits = std::allocator_traits<Alloc>;
public:
    using pointer = typename AllocTraits::pointer;
    using size_type = typename AllocTraits::size_type;
private:
    Alloc& alloc;
    size_type s;
public:
    AllocatorDestructor(Alloc& a, size_type s) noexcept : alloc(a), s(s) {}
    void operator()(pointer p) noexcept {AllocTraits::deallocate(alloc, p, s);}
};

template <typename T, typename VoidPtr> struct ForwardListNode;
template <typename NodePtr> struct ForwardBeginNode;

template <typename> struct ForwardListNodeValueType;

template <typename T, typename VoidPtr>
struct ForwardListNodeValueType<ForwardListNode<T, VoidPtr>> {
    using type = T;
};

template <typename NodePtr>
struct ForwardNodeTraits {
    using Node = typename std::remove_cv_t<typename std::pointer_traits<NodePtr>::element_type>;
    using NodeValueType = typename ForwardListNodeValueType<Node>::type;
    using NodePointer = NodePtr;
    using BeginNode = ForwardBeginNode<NodePtr>;
    using BeginNodePointer = typename std::pointer_traits<NodePtr>::template rebind<BeginNode>;
    using VoidPointer = typename std::pointer_traits<NodePtr>::template rebind<void>;
    using IterNodePointer = typename std::conditional_t<std::is_pointer_v<VoidPointer>, BeginNodePointer, NodePointer>;
    using NonIterNodePointer = typename std::conditional_t<std::is_same_v<IterNodePointer, NodePointer>, BeginNodePointer, NodePointer>;

    static IterNodePointer asIterNode(IterNodePointer p) {
        return p;
    }

    static IterNodePointer asIterNode(NonIterNodePointer p) {
        return static_cast<IterNodePointer>(static_cast<VoidPointer>(p));
    }
};

template <typename NodePtr>
struct ForwardBeginNode {
    using pointer = NodePtr;
    using BeginNodePointer = typename std::pointer_traits<NodePtr>::template rebind<ForwardBeginNode>;
    pointer next;

    ForwardBeginNode() : next(nullptr) {}

    BeginNodePointer nextAsBegin() const {
        return static_cast<BeginNodePointer>(next);
    }
};

template <typename T, typename VoidPtr>
struct BeginNodeOf {
    using type = typename ForwardBeginNode<typename std::pointer_traits<VoidPtr>::template rebind<ForwardListNode<T, VoidPtr>>>::type;
};

template <typename T, typename VoidPtr>
struct ForwardListNode : public BeginNodeOf<T, VoidPtr>::type {
    using value_type = T;
    value_type value;
};

template <typename T, typename Alloc = std::allocator<T>> class ForwardList;
template <typename NodeConstPtr> class ForwardListConstIterator;

template <typename NodePtr> class ForwardListIterator {
    using Traits = ForwardNodeTraits<NodePtr>;
    using NodePointer = typename Traits::NodePointer;
    using BeginNodePointer = typename Traits::BeginNodePointer;
    using IterNodePointer = typename Traits::IterNodePointer;
    using VoidPointer = typename Traits::VoidPointer;

    IterNodePointer ptr;

    BeginNodePointer getBegin() const {
        return static_cast<BeginNodePointer>(static_cast<VoidPointer>(ptr));
    }

    NodePointer getUnsafeNodePointer() const {
        return static_cast<NodePointer>(static_cast<VoidPointer>(ptr));
    }

    explicit ForwardListIterator(std::nullptr_t) noexcept : ptr(nullptr) {}

    explicit ForwardListIterator(BeginNodePointer p) noexcept : ptr(Traits::asIterNode(p)) {}

    explicit ForwardListIterator(NodePointer p) noexcept : ptr(Traits::asIterNode(p)) {}

    template <typename, typename> friend class ForwardList;
    template <typename> friend class ForwardListConstIterator;

public:

    using iterator_category = std::forward_iterator_tag;
    using value_type = typename Traits::NodeValueType;
    using reference = value_type&;
    using difference_type = typename std::pointer_traits<NodePointer>::difference_type;
    using pointer = typename std::pointer_traits<NodePointer>::template rebind<value_type>;

    ForwardListIterator() noexcept : ptr(nullptr) {}

    reference operator*() const {return getUnsafeNodePointer()->value;}

    pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(getUnsafeNodePointer()->value);}

    ForwardListIterator& operator++() {
        ptr = Traits::asIterNode(ptr->next);
        return *this;
    }

    ForwardListIterator operator++(int) {
        ForwardListIterator t(*this);
        ++(*this);
        return t;
    }

    friend bool operator==(const ForwardListIterator& x, const ForwardListIterator& y) {
        return x.ptr == y.ptr;
    }

    friend bool operator!=(const ForwardListIterator& x, const ForwardListIterator& y) {
        return !(x == y);
    }

};

template <typename NodeConstPtr>
class ForwardListConstIterator {
    static_assert(!std::is_const_v<typename std::pointer_traits<NodeConstPtr>::element_type>);
    using NodePtr = NodeConstPtr;

    using Traits = ForwardNodeTraits<NodePtr>;
    using Node = typename Traits::Node;
    using NodePointer = typename Traits::NodePointer;
    using BeginNodePointer = typename Traits::BeginNodePointer;
    using IterNodePointer = typename Traits::IterNodePointer;
    using VoidPointer = typename Traits::VoidPointer;

    IterNodePointer ptr;

    BeginNodePointer getBegin() const {
        return static_cast<BeginNodePointer>(static_cast<VoidPointer>(ptr));
    }
    NodePointer getUnsafeNodePointer() const {
        return static_cast<NodePointer>(static_cast<VoidPointer>(ptr));
    }

    explicit ForwardListConstIterator(std::nullptr_t) noexcept : ptr (nullptr) {}

    explicit ForwardListConstIterator(BeginNodePointer p) noexcept : ptr(Traits::asIterNode(p)) {}

    explicit ForwardListConstIterator(NodePointer p) noexcept : ptr(Traits::asIterNode(p)) {}

    template <typename, typename> friend class ForwardList;

public:

    using iterator_category = std::forward_iterator_tag;
    using value_type = typename Traits::NodeValueType;
    using reference = const value_type&;
    using difference_type = typename std::pointer_traits<NodePointer>::difference_type;
    using pointer = typename std::pointer_traits<NodePointer>::template rebind<const value_type>;

    ForwardListConstIterator() noexcept : ptr(nullptr) {}

    ForwardListConstIterator(ForwardListIterator<NodePointer> p) noexcept : ptr(p.ptr) {}

    reference operator*() const {return getUnsafeNodePointer()->value;}

    pointer operator->() const {return std::pointer_traits<pointer>::pointer_to(getUnsafeNodePointer()->value);}

    ForwardListConstIterator& operator++() {
        ptr = Traits::asIterNode(ptr->next);
        return *this;
    }

    ForwardListConstIterator operator++(int) {
        ForwardListConstIterator t(*this);
        ++(*this);
        return t;
    }

    friend bool operator==(const ForwardListConstIterator& x, const ForwardListConstIterator& y) {
        return x.ptr == y.ptr;
    }

    friend bool operator!=(const ForwardListConstIterator& x, const ForwardListConstIterator& y) {
        return !(x == y);
    }
};

template <typename T, typename Alloc>
class ForwardListBase {
protected:
    using value_type = T;
    using allocator_type = Alloc;

    using void_pointer = typename std::allocator_traits<allocator_type>::void_pointer;
    using Node = ForwardListNode<value_type, void_pointer>;
    using BeginNode = typename BeginNodeOf<value_type, void_pointer>::type;
    using NodeAllocator = typename std::allocator_traits<allocator_type>::template rebind_alloc<Node>;
    using NodeTraits = std::allocator_traits<NodeAllocator>;
    using NodePointer = typename NodeTraits::pointer;
    using BeginNodeAllocator = typename std::allocator_traits<allocator_type>::template rebind_alloc<BeginNode>;
    using BeginNodePointer = typename std::allocator_traits<BeginNodeAllocator>::pointer;

    static_assert(!std::is_same_v<allocator_type, NodeAllocator>,
                  "Internal allocator type must differ from user-specified type");

    std::pair<BeginNode, NodeAllocator> beforeBegin_;

    BeginNodePointer beforeBegin() noexcept {
        return std::pointer_traits<BeginNodePointer>::pointer_to(beforeBegin_.first);
    }

    BeginNodePointer beforeBegin() const noexcept {
        return std::pointer_traits<BeginNodePointer>::pointer_to(const_cast<BeginNode&>(beforeBegin_.first));
    }

    NodeAllocator& alloc() noexcept {
        return beforeBegin_.second;
    }

    const NodeAllocator& alloc() const noexcept {
        return beforeBegin_.second;
    }

    using iterator = ForwardListIterator<NodePointer>;
    using const_iterator = ForwardListConstIterator<NodePointer>;

    ForwardListBase() noexcept(std::is_nothrow_default_constructible_v<NodeAllocator>)
    : beforeBegin_(BeginNode(), NodeAllocator()) {}

    explicit ForwardListBase(const allocator_type &a)
            : beforeBegin_(BeginNode(), NodeAllocator(a)) {}

    explicit ForwardListBase(const NodeAllocator &a) : beforeBegin_(BeginNode(), a) {}

public:
    ForwardListBase(ForwardListBase&& x) noexcept(std::is_nothrow_move_constructible_v<NodeAllocator>);
    ForwardListBase(ForwardListBase&& x, const allocator_type& a);

private:
    ForwardListBase(const ForwardListBase&);
    ForwardListBase& operator=(const ForwardListBase&);

public:
    ~ForwardListBase();

protected:
    void copyAssignAlloc(const ForwardListBase& x) {
        copyAssignAlloc(x, std::integral_constant<bool, NodeTraits::propagate_on_container_copy_assignment::value>());
    }

    void moveAssignAlloc(ForwardListBase &x)
    noexcept(!NodeTraits::propagate_on_container_move_assignment::value ||
             std::is_nothrow_move_assignable_v<NodeAllocator>) {
        moveAssignAlloc(x, std::integral_constant<bool, NodeTraits::propagate_on_container_move_assignment::value>());
    }

    void clear() noexcept;

private:
    void copyAssignAlloc(const ForwardListBase&, std::false_type) {}
    void copyAssignAlloc(const ForwardListBase& x, std::true_type) {
        if (alloc() != x.alloc()) {
            clear();
        }
        alloc() = x.alloc();
    }

    void moveAssignAlloc(ForwardListBase&, std::false_type) noexcept {}
    void moveAssignAlloc(ForwardListBase& x, std::true_type) noexcept(std::is_nothrow_move_assignable_v<NodeAllocator>) {
        alloc() = std::move(x.alloc());
    }
};

template <typename T, typename Alloc>
inline ForwardListBase<T, Alloc>::ForwardListBase(ForwardListBase&& x)
noexcept(std::is_nothrow_move_constructible_v<NodeAllocator>) : beforeBegin_(std::move(x.beforeBegin_)) {
    x.beforeBegin()->next = nullptr;
}

template <typename T, typename Alloc>
inline ForwardListBase<T, Alloc>::ForwardListBase(ForwardListBase&& x, const allocator_type& a)
: beforeBegin_(BeginNode(), NodeAllocator(a)) {
    if (alloc() == x.alloc()) {
        beforeBegin()->next = x.beforeBegin()->next;
        x.beforeBegin()->next = nullptr;
    }
}

template <typename T, typename Alloc>
ForwardListBase<T, Alloc>::~ForwardListBase() {
    clear();
}

template <typename T, typename Alloc>
void ForwardListBase<T, Alloc>::clear() noexcept {
    NodeAllocator& a = alloc();
    for (NodePointer p = beforeBegin()->next; p != nullptr;) {
        NodePointer next = p->next;
        NodeTraits::destroy(a, std::addressof(p->value));
        NodeTraits::deallocate(a, p, 1);
        p = next;
    }
    beforeBegin()->next = nullptr;
}

template <typename T, typename Alloc>
class ForwardList : private ForwardListBase<T, Alloc> {
    using base = ForwardListBase<T, Alloc>;
    using NodeAllocator = typename base::NodeAllocator;
    using Node = typename base::Node;
    using NodeTraits = typename base::NodeTraits;
    using NodePointer = typename base::NodePointer;
    using BeginNodePointer = typename base::BeginNodePointer;

public:
    using value_type = T;
    using allocator_type = Alloc;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>);

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
    using size_type = typename std::allocator_traits<allocator_type>::size_type;
    using difference_type = typename std::allocator_traits<allocator_type>::difference_type;

    using iterator = typename base::iterator;
    using const_iterator = typename base::const_iterator;

    using RemoveReturnType = size_type;

    ForwardList() noexcept(std::is_nothrow_default_constructible_v<NodeAllocator>) {}

    explicit ForwardList(const allocator_type& a);
    explicit ForwardList(size_type n);
    explicit ForwardList(size_type n, const allocator_type& a);
    ForwardList(size_type n, const value_type& v);
    ForwardList(size_type n, const value_type& v, const allocator_type& a);

    template <typename InputIterator>
    ForwardList(InputIterator f, InputIterator l,
                typename std::enable_if_t<std::integral_constant<bool,
                        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                                std::input_iterator_tag>>::value> * = nullptr);

    template <typename InputIterator>
    ForwardList(InputIterator f, InputIterator l, const allocator_type& a,
                typename std::enable_if_t<std::integral_constant<bool,
                        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                                std::input_iterator_tag>>::value> * = nullptr);

    ForwardList(const ForwardList& x);
    ForwardList(const ForwardList& x, const allocator_type& a);

    ForwardList& operator=(const ForwardList& x);

    ForwardList(ForwardList&& x) noexcept(std::is_nothrow_move_constructible_v<base>) : base(std::move(x)) {}
    ForwardList(ForwardList&& x, const allocator_type& a);

    ForwardList(std::initializer_list<value_type> il);
    ForwardList(std::initializer_list<value_type> il, const allocator_type& a);

    ForwardList& operator=(ForwardList&& x)
            noexcept(NodeTraits::propagate_on_container_move_assignment::value
            && std::is_nothrow_move_assignable_v<allocator_type>);

    ForwardList& operator=(std::initializer_list<value_type> il);

    void assign(std::initializer_list<value_type> il);

    template <typename InputIterator>
    typename std::enable_if_t<std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                    std::input_iterator_tag>>::value, void> assign(InputIterator f, InputIterator l);

    void assign(size_type n, const value_type& v);

    allocator_type getAllocator() const noexcept {
        return allocator_type(base::alloc());
    }

    iterator begin() noexcept {return iterator(base::beforeBegin()->next); }
    const_iterator begin() const noexcept {return const_iterator(base::beforeBegin()->next); }
    iterator end() noexcept {return iterator(nullptr); }
    const_iterator end() const noexcept {return const_iterator(nullptr); }
    const_iterator cbegin() const noexcept {return iterator(base::beforeBegin()->next); }
    const_iterator cend() const noexcept {return const_iterator(nullptr); }
    iterator beforeBegin() noexcept { return iterator(base::beforeBegin()); }
    const_iterator beforeBegin() const noexcept { return const_iterator(base::beforeBegin()); }
    const_iterator cbeforeBegin() const noexcept { return const_iterator(base::beforeBegin()); }
    bool empty() const noexcept {return base::beforeBegin()->next == nullptr; }
    size_type maxSize() const noexcept { return std::min<size_type>(NodeTraits::maxSize(base::alloc()),
            std::numeric_limits<difference_type>::max());}

    reference front() { return base::beforeBegin()->next->value; }
    const_reference front() const { return base::beforeBegin()->next->value; }

    template <typename... Args>
    reference emplaceFront(Args&&... args);

    void pushFront(value_type&& v);
    void pushFront(const value_type& v);

    void popFront();

    template <typename... Args>
    iterator emplaceAfter(const_iterator p, Args&&... args);

    iterator insertAfter(const_iterator p, value_type&& v);
    iterator insertAfter(const_iterator p, std::initializer_list<value_type> il) {
        return insertAfter(p, il.begin(), il.end());
    }

    iterator insertAfter(const_iterator p, const value_type& v);
    iterator insertAfter(const_iterator p, size_type n, const value_type& v);

    template <typename InputIterator>
    typename std::enable_if_t<std::integral_constant<bool,
            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                    std::input_iterator_tag>>::value, iterator> insertAfter(const_iterator p, InputIterator f, InputIterator l);

    iterator eraseAfter(const_iterator p);
    iterator eraseAfter(const_iterator f, const_iterator l);

    void resize(size_type n);
    void resize(size_type n, const value_type& v);

    void clear() noexcept { base::clear(); }

    void spliceAfter(const_iterator p, ForwardList&& x);
    void spliceAfter(const_iterator p, ForwardList&& x, const_iterator i);
    void spliceAfter(const_iterator p, ForwardList&& x, const_iterator f, const_iterator l);
    void spliceAfter(const_iterator p, ForwardList& x);
    void spliceAfter(const_iterator p, ForwardList& x, const_iterator i);
    void spliceAfter(const_iterator p, ForwardList& x, const_iterator f, const_iterator l);

    RemoveReturnType remove(const value_type& v);
    template <typename Predicate>
    RemoveReturnType removeIf(Predicate pred);
    RemoveReturnType unique() {return unique(std::equal_to<void>());}
    template <typename BinaryPredicate>
    RemoveReturnType unique(BinaryPredicate binaryPred);

    void merge(ForwardList&& x) { merge(x, std::less<value_type>()); }
    template <typename Compare>
    void merge(ForwardList&& x, Compare comp) {merge (x, std::move(comp));}
    void merge(ForwardList& x) { merge(x, std::less<value_type>()); }
    template <typename Compare>
    void merge(ForwardList& x, Compare comp);

    void sort() {sort(std::less<value_type>());}
    template <typename Compare>
    void sort(Compare comp);

    void reverse() noexcept;

private:
    void moveAssign(ForwardList& x, std::true_type) noexcept(std::is_nothrow_move_assignable_v<allocator_type>);
    void moveAssign(ForwardList& x, std::false_type);

    template <typename Compare>
    static NodePointer Merge(NodePointer f1, NodePointer f2, Compare& comp);

    template <typename Compare>
    static NodePointer Sort(NodePointer f, difference_type sz, Compare& comp);

};

template<typename InputIterator,
        typename Alloc = typename std::allocator<typename std::iterator_traits<InputIterator>::value_type>,
        typename = typename std::enable_if_t<true, void>>
        ForwardList(InputIterator, InputIterator) -> ForwardList<typename std::iterator_traits<InputIterator>::value_type, Alloc>;

template<typename InputIterator,
        typename Alloc,
        typename = typename std::enable_if_t<true, void>>
ForwardList(InputIterator, InputIterator, Alloc) -> ForwardList<typename std::iterator_traits<InputIterator>::value_type, Alloc>;

template <typename T, typename Alloc>
inline ForwardList<T, Alloc>::ForwardList(const allocator_type& a) : base(a) {}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(size_type n) {
    if (n > 0) {
        NodeAllocator& a = base::alloc();
        using D = AllocatorDestructor<NodeAllocator>;
        UniquePtr<Node, D> h(nullptr, D(a, 1));
        for (BeginNodePointer p = base::beforeBegin(); n > 0; --n, p = p->nextAsBegin()) {
            h.reset(NodeTraits::allocate(a, 1));
            NodeTraits::construct(a, std::addressof(h->value));
            h->next = nullptr;
            p->next = h.release();
        }
    }
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(size_type n, const allocator_type& baseAlloc) : base (baseAlloc){
    if (n > 0) {
        NodeAllocator& a = base::alloc();
        using D = AllocatorDestructor<NodeAllocator>;
        UniquePtr<Node, D> h(nullptr, D(a, 1));
        for (BeginNodePointer p = base::beforeBegin(); n > 0; --n, p = p->nextAsBegin()) {
            h.reset(NodeTraits::allocate(a, 1));
            NodeTraits::construct(a, std::addressof(h->value));
            h->next = nullptr;
            p->next = h.release();
        }
    }
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(size_type n, const value_type& v) {
    insertAfter(cbeforeBegin(), n, v);
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(size_type n, const value_type& v, const allocator_type& a) : base(a) {
    insertAfter(cbeforeBegin(), n, v);
}

template <typename T, typename Alloc>
template <typename InputIterator>
ForwardList<T, Alloc>::ForwardList(InputIterator f, InputIterator l,
            typename std::enable_if_t<std::integral_constant<bool,
                    std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                            std::input_iterator_tag>>::value> *) {
    insertAfter(cbeforeBegin(), f, l);
}

template <typename T, typename Alloc>
template <typename InputIterator>
ForwardList<T, Alloc>::ForwardList(InputIterator f, InputIterator l, const allocator_type& a,
                                   typename std::enable_if_t<std::integral_constant<bool,
                                           std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                                                   std::input_iterator_tag>>::value> *) : base(a) {
    insertAfter(cbeforeBegin(), f, l);
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(const ForwardList& x) : base(NodeTraits::select_on_container_copy_construction(x.alloc())) {
    insertAfter(cbeforeBegin(), x.begin(), x.end());
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(const ForwardList& x, const allocator_type& a) : base(a) {
    insertAfter(cbeforeBegin(), x.begin(), x.end());
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>& ForwardList<T, Alloc>::operator=(const ForwardList& x) {
    if (this != x) {
        base::copyAssignAlloc(x);
        assign(x.begin(), x.end());
    }
    return *this;
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(ForwardList&& x, const allocator_type& a) : base(std::move(x), a) {
    if (base::alloc() != x.alloc()) {
        insertAfter(cbeforeBegin(), std::move_iterator<iterator>(x.begin()), std::move_iterator<iterator>(x.end()));
    }
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(std::initializer_list<value_type> il) {
    insertAfter(cbeforeBegin(), il.begin(), il.end());
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(std::initializer_list<value_type> il, const allocator_type& a) : base(a) {
    insertAfter(cbeforeBegin(), il.begin(), il.end());
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::moveAssign(ForwardList& x, std::true_type) noexcept(std::is_nothrow_move_assignable_v<allocator_type>) {
    clear();
    base::moveAssignAlloc(x);
    base::beforeBegin()->next = x.beforeBegin()->next;
    x.beforeBegin()->next = nullptr;
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::moveAssign(ForwardList& x, std::false_type) {
    if (base::alloc() == x.alloc()) {
        moveAssign(x, std::true_type());
    } else {
        assign(std::move_iterator<iterator>(x.begin()), std::move_iterator<iterator>(x.end()));
    }
}

template <typename T, typename Alloc>
inline ForwardList<T, Alloc>& ForwardList<T, Alloc>::operator=(ForwardList &&x)
noexcept(NodeTraits::propagate_on_container_move_assignment::value &&
         std::is_nothrow_move_assignable_v<allocator_type>) {
    moveAssign(x, std::integral_constant<bool, NodeTraits::propagate_on_container_move_assignment::value>());
    return *this;
}

template <typename T, typename Alloc>
inline ForwardList<T, Alloc>& ForwardList<T, Alloc>::operator=(std::initializer_list<value_type> il) {
    assign(il.begin(), il.end());
    return *this;
}

template <typename T, typename Alloc>
template <typename InputIterator>
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                std::input_iterator_tag>>::value, void>
ForwardList<T, Alloc>::assign(InputIterator f, InputIterator l) {
    iterator i = beforeBegin();
    iterator j = std::next(i);
    iterator e = end();
    for (; j != e && f != l; ++i, (void) ++j, ++f) {
        *j = *f;
    }
    if (j == e) {
        insertAfter(i, f, l);
    } else {
        eraseAfter(i, e);
    }
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::assign(size_type n, const value_type& v) {
    iterator i = beforeBegin();
    iterator j = std::next(i);
    iterator e = end();
    for (; j != e && n > 0; --n, ++i, ++j) {
        *j = *v;
    }
    if (j == e) {
        insertAfter(i, n, v);
    } else {
        eraseAfter(i, e);
    }
}

template <typename T, typename Alloc>
inline void ForwardList<T, Alloc>::assign(std::initializer_list<value_type> il) {
    assign(il.begin(), il.end());
}

template <typename T, typename Alloc>
template <typename... Args>
typename ForwardList<T, Alloc>::reference ForwardList<T, Alloc>::emplaceFront(Args&&... args) {
    NodeAllocator& a = base::alloc();
    using D = AllocatorDestructor<NodeAllocator>;
    UniquePtr<Node, D> h(NodeTraits::allocate(a, 1), D(a, 1));
    NodeTraits::construct(a, std::addressof(h->value), std::forward<Args>(args)...);
    h->next = base::beforeBegin()->next;
    base::beforeBegin()->next = h.release();
    return base::beforeBegin()->next->value;
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::pushFront(value_type&& v) {
    NodeAllocator& a = base::alloc();
    using D = AllocatorDestructor<NodeAllocator>;
    UniquePtr<Node, D> h(NodeTraits::allocate(a, 1), D(a, 1));
    NodeTraits::construct(a, std::addressof(h->value), std::move(v));
    h->next = base::beforeBegin()->next;
    base::beforeBegin()->next = h.release();
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::pushFront(const value_type& v) {
    NodeAllocator& a = base::alloc();
    using D = AllocatorDestructor<NodeAllocator>;
    UniquePtr<Node, D> h(NodeTraits::allocate(a, 1), D(a, 1));
    NodeTraits::construct(a, std::addressof(h->value), v);
    h->next = base::beforeBegin()->next;
    base::beforeBegin()->next = h.release();
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::popFront() {
    NodeAllocator& a = base::alloc();
    NodePointer p = base::beforeBegin()->next;
    base::beforeBegin()->next = p->next;
    NodeTraits::destroy(a, std::addressof(p->value));
    NodeTraits::deallocate(a, p, 1);
}

template <typename T, typename Alloc>
template <typename... Args>
typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::emplaceAfter(const_iterator p, Args&&... args) {
    BeginNodePointer const r = p.getBegin();
    NodeAllocator& a = base::alloc();
    using D = AllocatorDestructor<NodeAllocator>;
    UniquePtr<Node, D> h(NodeTraits::allocate(a, 1), D(a, 1));
    NodeTraits::construct(a, std::addressof(h->value), std::forward<Args>(args)...);
    h->next = r->next;
    r->next = h.release();
    return iterator(r->next);
}

template <typename T, typename Alloc>
typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::insertAfter(const_iterator p, value_type&& v) {
    BeginNodePointer const r = p.getBegin();
    NodeAllocator& a = base::alloc();
    using D = AllocatorDestructor<NodeAllocator>;
    UniquePtr<Node, D> h(NodeTraits::allocate(a, 1), D(a, 1));
    NodeTraits::construct(a, std::addressof(h->value), std::move(v));
    h->next = r->next;
    r->next = h.release();
    return iterator(r->next);
}

template <typename T, typename Alloc>
typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::insertAfter(const_iterator p, const value_type& v) {
    BeginNodePointer const r = p.getBegin();
    NodeAllocator& a = base::alloc();
    using D = AllocatorDestructor<NodeAllocator>;
    UniquePtr<Node, D> h(NodeTraits::allocate(a, 1), D(a, 1));
    NodeTraits::construct(a, std::addressof(h->value), v);
    h->next = r->next;
    r->next = h.release();
    return iterator(r->next);
}

template <typename T, typename Alloc>
typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::insertAfter(const_iterator p, size_type n, const value_type& v) {
    BeginNodePointer r = p.getBegin();
    if (n > 0) {
        NodeAllocator& a = base::alloc();
        using D = AllocatorDestructor<NodeAllocator>;
        UniquePtr<Node, D> h(NodeTraits::allocate(a, 1), D(a, 1));
        NodeTraits::construct(a, std::addressof(h->value), v);
        NodePointer first = h.release();
        NodePointer last = first;
        try {
            for (--n; n != 0; --n, last = last->next) {
                h.reset(NodeTraits::allocate(a, 1));
                NodeTraits::construct(a, std::addressof(h->value), v);
                last->next = h.release();
            }
        } catch (...) {
            while (first != nullptr) {
                NodePointer next = first->next;
                NodeTraits::destroy(a, std::addressof(first->value));
                NodeTraits::deallocate(a, first, 1);
                first = next;
            }
            throw;
        }
        last->next = r->next;
        r->next = first;
        r = static_cast<BeginNodePointer>(last);
    }
    return iterator(r);
}

template<typename T, typename Alloc>
template<typename InputIterator>
typename std::enable_if_t<std::integral_constant<bool,
        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                std::input_iterator_tag>>::value, typename ForwardList<T, Alloc>::iterator>
ForwardList<T, Alloc>::insertAfter(const_iterator p, InputIterator f, InputIterator l) {
    BeginNodePointer r = p.getBegin();
    if (f != l) {
        NodeAllocator& a = base::alloc();
        using D = AllocatorDestructor<NodeAllocator>;
        UniquePtr<Node, D> h(NodeTraits::allocate(a, 1), D(a, 1));
        NodeTraits::construct(a, std::addressof(h->value), *f);
        NodePointer first = h.release();
        NodePointer last = first;
        try {
            for (++f; f != l; ++f, ((void)(last = last->next))) {
                h.reset(NodeTraits::allocate(a, 1));
                NodeTraits::construct(a, std::addressof(h->value), *f);
                last->next = h.release();
            }
        } catch (...) {
            while (first != nullptr) {
                NodePointer next = first->next;
                NodeTraits::destroy(a, std::addressof(first->value));
                NodeTraits::deallocate(a, first, 1);
                first = next;
            }
            throw;
        }
        last->next = r->next;
        r->next = first;
        r = static_cast<BeginNodePointer>(last);
    }
    return iterator(r);
}

template<typename T, typename Alloc>
typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::eraseAfter(const_iterator f) {
    BeginNodePointer p = f.getBegin();
    NodePointer n = p->next;
    p->next = n->next;
    NodeAllocator& a = base::alloc();
    NodeTraits::destroy(a, std::addressof(n->value));
    NodeTraits::deallocate(a, n, 1);
    return iterator(p->next);
}

template<typename T, typename Alloc>
typename ForwardList<T, Alloc>::iterator ForwardList<T, Alloc>::eraseAfter(const_iterator f, const_iterator l) {
    NodePointer e = l.getUnsafeNodePointer();
    if (f != l) {
        BeginNodePointer bp = f.getBegin();
        NodePointer n = bp->next;
        if (n != e) {
            bp->next = e;
            NodeAllocator& a = base::alloc();
            do {
                NodePointer tmp = n->next;
                NodeTraits::destroy(a, std::addressof(n->value));
                NodeTraits::deallocate(a, n, 1);
                n = tmp;
            } while (n != e);

        }
    }
    return iterator(e);
}

template<typename T, typename Alloc>
void ForwardList<T, Alloc>::resize(size_type n) {
    size_type sz = 0;
    iterator p = beforeBegin();
    iterator i = begin();
    iterator e = end();
    for (; i != e && sz < n; ++p, ++i, ++sz) ;
    if (i != e) {
        eraseAfter(p, e);
    } else {
        n -= sz;
        if (n > 0) {
            NodeAllocator& a = base::alloc();
            using D = AllocatorDestructor<NodeAllocator>;
            UniquePtr<Node, D> h(nullptr, D(a, 1));
            for (BeginNodePointer ptr = p.getBegin(); n > 0; --n, ptr = ptr->nextAsBegin()) {
                h.reset(NodeTraits::allocate(a, 1));
                NodeTraits::construct(a, std::addressof(h->value));
                h->next = nullptr;
                ptr->next = h.release();
            }
        }
    }
}

template<typename T, typename Alloc>
void ForwardList<T, Alloc>::resize(size_type n, const value_type& v) {
    size_type sz = 0;
    iterator p = beforeBegin();
    iterator i = begin();
    iterator e = end();
    for (; i != e && sz < n; ++p, ++i, ++sz) ;
    if (i != e) {
        eraseAfter(p, e);
    } else {
        n -= sz;
        if (n > 0) {
            NodeAllocator& a = base::alloc();
            using D = AllocatorDestructor<NodeAllocator>;
            UniquePtr<Node, D> h(nullptr, D(a, 1));
            for (BeginNodePointer ptr = p.getBegin(); n > 0; --n, ptr = ptr->nextAsBegin()) {
                h.reset(NodeTraits::allocate(a, 1));
                NodeTraits::construct(a, std::addressof(h->value), v);
                h->next = nullptr;
                ptr->next = h.release();
            }
        }
    }
}

template<typename T, typename Alloc>
void ForwardList<T, Alloc>::spliceAfter(const_iterator p, ForwardList& x) {
    if (!x.empty()) {
        if (p.getBegin()->next != nullptr) {
            const_iterator lm1 = x.beforeBegin();
            while (lm1.getBegin()->next != nullptr) {
                ++lm1;
            }
            lm1.getBegin()->next = p.getBegin()->next;
        }
        p.getBegin()->next = x.beforeBegin()->next;
        x.beforeBegin()->next = nullptr;
    }
}

template<typename T, typename Alloc>
void ForwardList<T, Alloc>::spliceAfter(const_iterator p, ForwardList&, const_iterator i) {
    const_iterator lm1 = std::next(i);
    if (p != i && p != lm1) {
        i.getBegin()->next = lm1.getBegin()->next;
        lm1.getBegin()->next = p.getBegin()->next;
        p.getBegin()->next = lm1.getUnsafeNodePointer();
    }
}

template<typename T, typename Alloc>
void ForwardList<T, Alloc>::spliceAfter(const_iterator p, ForwardList&, const_iterator f, const_iterator l) {
    if (f != l && p != f) {
        const_iterator lm1 = f;
        while (lm1.getBegin()->next != l.getBegin()) {
            ++lm1;
        }
        if (f != lm1) {
            lm1.getBegin()->next = p.getBegin()->next;
            p.getBegin()->next = f.getBegin()->next;
            f.getBegin()->next = l.getUnsafeNodePointer();
        }
    }
}

template<typename T, typename Alloc>
inline void ForwardList<T, Alloc>::spliceAfter(const_iterator p, ForwardList&& x) {
    spliceAfter(p, x);
}

template<typename T, typename Alloc>
inline void ForwardList<T, Alloc>::spliceAfter(const_iterator p, ForwardList&& x, const_iterator i) {
    spliceAfter(p, x, i);
}

template<typename T, typename Alloc>
inline void ForwardList<T, Alloc>::spliceAfter(const_iterator p, ForwardList&& x, const_iterator f, const_iterator l) {
    spliceAfter(p, x, f, l);
}

template<typename T, typename Alloc>
typename ForwardList<T, Alloc>::RemoveReturnType ForwardList<T, Alloc>::remove(const value_type& v) {
    ForwardList<T, Alloc> deletedNodes(getAllocator());
    typename ForwardList<T, Alloc>::size_type countRemoved = 0;
    const iterator e = end();
    for (iterator i = beforeBegin(); i.getBegin()->next != nullptr; ) {
        if (i.getBegin()->next->value == v) {
            ++countRemoved;
            iterator j = std::next(i, 2);
            for (; j != e && *j == v; ++j) {
                ++countRemoved;
            }

            deletedNodes.spliceAfter(deletedNodes.beforeBegin(), *this, i, j);
            if (j == e) break;
            i = j;
        } else {
            ++i;
        }
    }
    return (RemoveReturnType) countRemoved;
}

template<typename T, typename Alloc>
template<typename Predicate>
typename ForwardList<T, Alloc>::RemoveReturnType ForwardList<T, Alloc>::removeIf(Predicate pred) {
    ForwardList<T, Alloc> deletedNodes(getAllocator());
    typename ForwardList<T, Alloc>::size_type countRemoved = 0;
    const iterator e = end();
    for (iterator i = beforeBegin(); i.getBegin()->next != nullptr; ) {
        if (pred(i.getBegin()->next->value)) {
            ++countRemoved;
            iterator j = std::next(i, 2);
            for (; j != e && *j == pred(*j); ++j) {
                ++countRemoved;
            }

            deletedNodes.spliceAfter(deletedNodes.beforeBegin(), *this, i, j);
            if (j == e) break;
            i = j;
        } else {
            ++i;
        }
    }
    return (RemoveReturnType) countRemoved;
}

template<typename T, typename Alloc>
template<typename BinaryPredicate>
typename ForwardList<T, Alloc>::RemoveReturnType ForwardList<T, Alloc>::unique(BinaryPredicate binaryPred) {
    ForwardList<T, Alloc> deletedNodes(getAllocator());
    typename ForwardList<T, Alloc>::size_type countRemoved = 0;
    for (iterator i = begin(), e = end(); i != e;) {
        iterator j = std::next(i);
        for (; j != e && binaryPred(*i, *j); ++j) {
            ++countRemoved;
        }
        if (i.getBegin()->next != j.getUnsafeNodePointer()) {
            deletedNodes.spliceAfter(deletedNodes.beforeBegin(), *this, i, j);
        }
        i = j;
    }
    return (RemoveReturnType) countRemoved;
}

template<typename T, typename Alloc>
template<typename Compare>
void ForwardList<T, Alloc>::merge(ForwardList& x, Compare comp) {
    if (this != &x) {
        base::beforeBegin()->next = Merge(base::beforeBegin()->next, x.beforeBegin()->next, comp);
        x.beforeBegin()->next = nullptr;
    }
}

template<typename T, typename Alloc>
template<typename Compare>
typename ForwardList<T, Alloc>::NodePointer ForwardList<T, Alloc>::Merge(NodePointer f1, NodePointer f2, Compare &comp) {
    if (f1 == nullptr) return f2;
    if (f2 == nullptr) return f1;
    NodePointer r;

    if (comp(f2->value, f1->value)) {
        NodePointer t = f2;
        while (t->next != nullptr && comp(t->next->value, f1->value)) {
            t = t->next;
        }
        r = f2;
        f2 = t->next;
        t->next = f1;
    } else {
        r = f1;
    }

    NodePointer p = f1;
    f1 = f1->next;
    while (f1 != nullptr && f2 != nullptr) {
        if (comp(f2->value, f1->value)) {
            NodePointer t = f2;
            while (t->next != nullptr && comp(t->next->value, f1->value)) {
                t = t->next;
            }
            p->next = f2;
            f2 = t->next;
            t->next = f1;
        }
        p = f1;
        f1 = f1->next;
    }
    if (f2 != nullptr) {
        p->next = f2;
    }
    return r;
}

template<typename T, typename Alloc>
template<typename Compare>
inline void ForwardList<T, Alloc>::sort(Compare comp) {
    base::beforeBegin()->next = Sort(base::beforeBegin()->next, std::distance(begin(), end()), comp);
}

template<typename T, typename Alloc>
template<typename Compare>
typename ForwardList<T, Alloc>::NodePointer ForwardList<T, Alloc>::Sort(NodePointer f1, difference_type sz, Compare& comp) {
    switch (sz) {
        case 0:
        case 1:
            return f1;
        case 2:
            if (comp(f1->next->value, f1->value)) {
                NodePointer t = f1->next;
                t->next = f1;
                f1->next = nullptr;
                f1 = t;
            }
            return f1;
    }
    difference_type sz1 = sz / 2;
    difference_type sz2 = sz - sz1;
    NodePointer t = std::next(iterator(f1), sz1 - 1).getUnsafeNodePointer();
    NodePointer f2 = t->next;
    t->next = nullptr;
    return Merge(Sort(f1, sz1, comp), Sort(f2, sz2, comp), comp);
}

template<typename T, typename Alloc>
void ForwardList<T, Alloc>::reverse() noexcept {
    NodePointer p = base::beforeBegin()->next;
    if (p != nullptr) {
        NodePointer f = p->next;
        p->next = nullptr;
        while (f != nullptr) {
            NodePointer t = f->next;
            f->next = p;
            p = f;
            f = t;
        }
        base::beforeBegin()->next = p;
    }
}

template<typename T, typename Alloc>
bool operator==(const ForwardList<T, Alloc>& x, const ForwardList<T, Alloc>& y) {
    using Ip = typename ForwardList<T, Alloc>::const_iterator;
    Ip ix = x.begin();
    Ip ex = x.end();
    Ip iy = y.begin();
    Ip ey = y.end();
    for (; ix != ex && iy != ey; ++ix, +iy) {
        if (!(*ix == *iy)) {
            return false;
        }
    }
    return (ix == ex) == (iy == ey);
}

template<typename T, typename Alloc>
bool operator!=(const ForwardList<T, Alloc>& x, const ForwardList<T, Alloc>& y) {
    return !(x == y);
}

template<typename T, typename Alloc>
bool operator<(const ForwardList<T, Alloc>& x, const ForwardList<T, Alloc>& y) {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<typename T, typename Alloc>
bool operator>(const ForwardList<T, Alloc>& x, const ForwardList<T, Alloc>& y) {
    return y < x;
}

template<typename T, typename Alloc>
bool operator>=(const ForwardList<T, Alloc>& x, const ForwardList<T, Alloc>& y) {
    return !(x < y);
}

template<typename T, typename Alloc>
bool operator<=(const ForwardList<T, Alloc>& x, const ForwardList<T, Alloc>& y) {
    return !(y < x);
}

template<typename T, typename Alloc, typename Predicate>
inline void eraseIf(ForwardList<T, Alloc>& c, Predicate pred) {
    c.removeIf(pred);
}

template<typename T, typename Alloc, typename U>
inline void erase(ForwardList<T, Alloc>& c, const U& v) {
    eraseIf(c, [&](auto& elem) {return elem == v;});
}

int main() {

}