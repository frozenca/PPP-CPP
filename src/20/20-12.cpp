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

template <typename T, typename VoidPtr> struct ListNode;
template <typename T, typename VoidPtr> struct ListNodeBase;

template <typename T, typename VoidPtr>
struct ListNodePointerTraits {
    using NodePointer = typename std::pointer_traits<VoidPtr>::template rebind<ListNode<T, VoidPtr>>;
    using BasePointer = typename std::pointer_traits<VoidPtr>::template rebind<ListNodeBase<T, VoidPtr>>;
    using LinkPointer = typename std::conditional_t<std::is_pointer_v<VoidPtr>, BasePointer, NodePointer>;
    using NonLinkPointer = typename std::conditional_t<std::is_same_v<LinkPointer, NodePointer>, BasePointer, NodePointer>;

    static LinkPointer unsafeLinkPointerCast(LinkPointer p) {
        return p;
    }

    static LinkPointer unsafeLinkPointerCast(NonLinkPointer p) {
        return static_cast<LinkPointer>(static_cast<VoidPtr>(p));
    }
};

template <typename T, typename VoidPtr>
struct ListNodeBase {
    using NodeTraits = ListNodePointerTraits<T, VoidPtr>;
    using NodePointer = typename NodeTraits::NodePointer;
    using BasePointer = typename NodeTraits::BasePointer;
    using LinkPointer = typename NodeTraits::LinkPointer;

    LinkPointer prev;
    LinkPointer next;

    ListNodeBase() : prev(NodeTraits::unsafeLinkPointerCast(self())), next(NodeTraits::unsafeLinkPointerCast(self())) {}

    BasePointer self() {
        return std::pointer_traits<BasePointer>::pointer_to(*this);
    }

    NodePointer asNode() {
        return static_cast<NodePointer>(self());
    }
};

template <typename T, typename VoidPtr>
struct ListNode : public ListNodeBase<T, VoidPtr> {
    T value;
    using Base = ListNodeBase<T, VoidPtr>;
    using LinkPointer = typename Base::LinkPointer;

    LinkPointer asLink() {
        return static_cast<LinkPointer>(Base::self());
    }
};

template <typename T, typename Alloc = std::allocator<T>> class List;
template <typename T, typename Alloc> class ListImpl;
template <typename T, typename VoidPtr> class ListConstIterator;

template <typename T, typename VoidPtr> class ListIterator {
    using NodeTraits = ListNodePointerTraits<T, VoidPtr>;
    using LinkPointer = typename NodeTraits::LinkPointer;

    LinkPointer ptr;

    explicit ListIterator(LinkPointer p) noexcept : ptr (p) {}

    template <typename, typename> friend class List;
    template <typename, typename> friend class ListImpl;
    template <typename, typename> friend class ListConstIterator;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using reference = value_type&;
    using pointer = typename std::pointer_traits<VoidPtr>::template rebind<value_type>;
    using difference_type = typename std::pointer_traits<pointer>::difference_type;

    ListIterator() noexcept : ptr (nullptr) {}

    reference operator*() const {
        return ptr->asNode()->value;
    }

    pointer operator->() const {
        return std::pointer_traits<pointer>::pointer_to(ptr->asNode()->value);
    }

    ListIterator& operator++() {
        ptr = ptr->next;
        return *this;
    }

    ListIterator operator++(int) {
        ListIterator t(*this);
        ++(*this);
        return t;
    }

    ListIterator& operator--() {
        ptr = ptr->prev;
        return *this;
    }

    ListIterator operator--(int) {
        ListIterator t(*this);
        --(*this);
        return t;
    }

    friend bool operator==(const ListIterator& x, const ListIterator& y) {
        return x.ptr == y.ptr;
    }

    friend bool operator!=(const ListIterator& x, const ListIterator& y) {
        return !(x == y);
    }
};

template <typename T, typename VoidPtr> class ListConstIterator {
    using NodeTraits = ListNodePointerTraits<T, VoidPtr>;
    using LinkPointer = typename NodeTraits::LinkPointer;

    LinkPointer ptr;

    explicit ListConstIterator(LinkPointer p) noexcept : ptr (p) {}

    template <typename, typename> friend class List;
    template <typename, typename> friend class ListImpl;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using reference = value_type&;
    using pointer = typename std::pointer_traits<VoidPtr>::template rebind<const value_type>;
    using difference_type = typename std::pointer_traits<pointer>::difference_type;

    ListConstIterator() noexcept : ptr (nullptr) {}

    ListConstIterator(const ListIterator<T, VoidPtr>& p) noexcept : ptr (p.ptr) {}

    reference operator*() const {
        return ptr->asNode()->value;
    }

    pointer operator->() const {
        return std::pointer_traits<pointer>::pointer_to(ptr->asNode()->value);
    }

    ListConstIterator& operator++() {
        ptr = ptr->next;
        return *this;
    }

    ListConstIterator operator++(int) {
        ListConstIterator t(*this);
        ++(*this);
        return t;
    }

    ListConstIterator& operator--() {
        ptr = ptr->prev;
        return *this;
    }

    ListConstIterator operator--(int) {
        ListConstIterator t(*this);
        --(*this);
        return t;
    }

    friend bool operator==(const ListConstIterator& x, const ListConstIterator& y) {
        return x.ptr == y.ptr;
    }

    friend bool operator!=(const ListConstIterator& x, const ListConstIterator& y) {
        return !(x == y);
    }
};

template <typename T, typename Alloc>
class ListImpl {
    ListImpl (const ListImpl&);
    ListImpl& operator= (const ListImpl&);
public:
    using allocator_type = Alloc;
    using AllocTraits = std::allocator_traits<allocator_type>;
    using size_type = typename AllocTraits::size_type;

protected:
    using value_type = T;
    using VoidPointer = typename AllocTraits::void_pointer;
    using iterator = ListIterator<value_type, VoidPointer>;
    using const_iterator = ListConstIterator<value_type, VoidPointer>;
    using NodeBase = ListNodeBase<value_type, VoidPointer>;
    using Node = ListNode<value_type, VoidPointer>;
    using NodeAllocator = typename AllocTraits::template rebind_alloc<Node>;
    using NodeAllocTraits = std::allocator_traits<NodeAllocator>;
    using NodePointer = typename NodeAllocTraits::pointer;
    using NodeConstPointer = typename NodeAllocTraits::pointer;
    using NodePointerTraits = ListNodePointerTraits<value_type, VoidPointer>;
    using LinkPointer = typename NodePointerTraits::LinkPointer;
    using LinkConstPointer = LinkPointer;
    using pointer = typename AllocTraits::pointer;
    using const_pointer = typename AllocTraits::const_pointer;
    using difference_type = typename AllocTraits::difference_type;
    using NodeBaseAllocator = typename AllocTraits::template rebind_alloc<NodeBase>;
    using NodeBasePointer = typename std::allocator_traits<NodeBaseAllocator>::pointer;
    static_assert(!std::is_same_v<allocator_type, NodeAllocator>,
            "Internal allocator type must differ from user-specified allocator type");

    NodeBase end_;
    std::pair<size_type, NodeAllocator> sizeAlloc;

    LinkPointer endAsLink() const noexcept {
        return NodePointerTraits::unsafeLinkPointerCast(const_cast<NodeBase&>(end_).self());
    }

    size_type& sz() noexcept {return sizeAlloc.first;}
    const size_type& sz() const noexcept {return sizeAlloc.first;}
    NodeAllocator& nodeAlloc() noexcept {return sizeAlloc.second;}
    const NodeAllocator& nodeAlloc() const noexcept {return sizeAlloc.second;}
    size_type nodeAllocMaxSize() const noexcept { return NodeAllocTraits::max_size(nodeAlloc()); }

    static void unlinkNodes(LinkPointer f, LinkPointer l) noexcept;

    ListImpl() noexcept(std::is_nothrow_default_constructible_v<NodeAllocator>);

    ListImpl(const allocator_type& a);
    ListImpl(const NodeAllocator& a);
    ListImpl(NodeAllocator&& a) noexcept;
    ~ListImpl();
    void clear() noexcept;
    bool empty() const noexcept {return sz() == 0;}

    iterator begin() noexcept {
        return iterator(end_.next);
    }

    const_iterator begin() const noexcept {
        return const_iterator(end_.next);
    }

    iterator end() noexcept {
        return iterator(endAsLink());
    }

    const_iterator end() const noexcept {
        return const_iterator(endAsLink());
    }

    void copyAssignAlloc(const ListImpl& c) {
        copyAssignAlloc(c, std::integral_constant<bool, NodeAllocTraits::propagate_on_container_copy_assignment::value>());
    }

    void moveAssignAlloc(ListImpl& c) noexcept (!NodeAllocTraits::propagate_on_container_move_assignment::value
    || std::is_nothrow_move_assignable_v<NodeAllocator>){
        moveAssignAlloc(c, std::integral_constant<bool, NodeAllocTraits::propagate_on_container_move_assignment::value>());
    }

private:
    void copyAssignAlloc(const ListImpl& c, std::true_type) {
        if (nodeAlloc() != c.nodeAlloc()) {
            clear();
        }
        nodeAlloc() = c.nodeAlloc();
    }

    void copyAssignAlloc(const ListImpl& c, std::false_type) {}

    void moveAssignAlloc(ListImpl& c, std::true_type) noexcept (std::is_nothrow_move_assignable_v<NodeAllocator>) {
        nodeAlloc() = std::move(c.nodeAlloc());
    }

    void moveAssignAlloc(ListImpl& c, std::false_type) noexcept {}

    void invalidateAllIterators() {}

};

template <typename T, typename Alloc>
inline void ListImpl<T, Alloc>::unlinkNodes(LinkPointer f, LinkPointer l) noexcept {
    f->prev->next = l->next;
    l->next->prev = f->prev;
}

template <typename T, typename Alloc>
inline ListImpl<T, Alloc>::ListImpl() noexcept(std::is_nothrow_default_constructible_v<NodeAllocator>)
: sizeAlloc(0, Alloc())
{}

template <typename T, typename Alloc>
inline ListImpl<T, Alloc>::ListImpl(const allocator_type& a) : sizeAlloc(0, NodeAllocator(a))
{}

template <typename T, typename Alloc>
inline ListImpl<T, Alloc>::ListImpl(const NodeAllocator& a) : sizeAlloc(0, a)
{}

template <typename T, typename Alloc>
inline ListImpl<T, Alloc>::ListImpl(NodeAllocator&& a) noexcept : sizeAlloc(0, std::move(a))
{}

template <typename T, typename Alloc>
ListImpl<T, Alloc>::~ListImpl<T, Alloc>() {
    clear();
}

template <typename T, typename Alloc>
void ListImpl<T, Alloc>::clear() noexcept {
    if (!empty()) {
        NodeAllocator& na = nodeAlloc();
        LinkPointer f = end_.next;
        LinkPointer l = endAsLink();
        unlinkNodes(f, l->prev);
        sz() = 0;
        while (f != l) {
            NodePointer np = f->asNode();
            f = f->next;
            NodeAllocTraits::destroy(na, std::addressof(np->value));
            NodeAllocTraits::deallocate(na, np, 1);
        }
        invalidateAllIterators();
    }
}

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

template <typename T, typename Alloc>
class List : private ListImpl<T, Alloc> {
    using base = ListImpl<T, Alloc>;
    using Node = typename base::Node;
    using NodeAllocator = typename base::NodeAllocator;
    using NodePointer = typename base::NodePointer;
    using NodeAllocTraits = typename base::NodeAllocTraits;
    using NodeBase = typename base::NodeBase;
    using NodeBasePointer = typename base::NodeBasePointer;
    using LinkPointer = typename base::LinkPointer;

public:
    using value_type = T;
    using allocator_type = Alloc;
    static_assert(std::is_same_v<value_type, typename allocator_type::value_type>);
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename base::pointer;
    using const_pointer = typename base::const_pointer;
    using size_type = typename base::size_type;
    using difference_type = typename base::difference_type;
    using iterator = typename base::iterator;
    using const_iterator = typename base::const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using RemoveReturnType = size_type;

    List() noexcept(std::is_nothrow_default_constructible_v<NodeAllocator>) {}

    explicit List(const allocator_type& a) : base(a) {}

    explicit List(size_type n);

    explicit List(size_type n, const allocator_type& a);

    List(size_type n, const value_type& x);

    List(size_type n, const value_type& x, const allocator_type& a);

    template <typename InputIterator>
    List(InputIterator f, InputIterator l,
         typename std::enable_if_t<std::integral_constant<bool,
                 std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                 std::input_iterator_tag>>::value, void> * = 0);

    template <typename InputIterator>
    List(InputIterator f, InputIterator l, const allocator_type& a,
         typename std::enable_if_t<std::integral_constant<bool,
                 std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                         std::input_iterator_tag>>::value, void> * = 0);

    List (const List& c);
    List (const List& c, const allocator_type& a);
    List& operator=(const List& c);
    List(std::initializer_list<value_type> il);
    List(std::initializer_list<value_type> il, const allocator_type& a);

    List(List&& c) noexcept(std::is_nothrow_move_constructible_v<NodeAllocator>);
    List(List&& c, const allocator_type& a);
    List& operator=(List&& c) noexcept(NodeAllocTraits::propagate_on_container_move_assignment::value
            && std::is_nothrow_move_assignable_v<NodeAllocator>);

    List& operator=(std::initializer_list<value_type> il) {
        assign(il.begin(), il.end());
        return *this;
    }

    void assign(std::initializer_list<value_type> il) {
        assign(il.begin(), il.end());
    }

    template <typename InputIterator>
    void assign(InputIterator f, InputIterator l,
                typename std::enable_if_t<std::integral_constant<bool,
                        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                                std::input_iterator_tag>>::value, void> * = 0);

    void assign(size_type n, const value_type& x);

    allocator_type getAllocator() const noexcept;

    size_type size() const noexcept {return base::sz();}

    bool empty() const noexcept {return base::empty();}

    size_type maxSize() const noexcept {
        return std::min<size_type>(base::nodeAllocMaxSize(), std::numeric_limits<difference_type>::max());
    }

    iterator begin() noexcept { return base::begin();}
    const_iterator begin() const noexcept { return base::begin();}
    iterator end() noexcept { return base::end();}
    const_iterator end() const noexcept { return base::end();}
    const_iterator cbegin() const noexcept { return base::begin();}
    const_iterator cend() const noexcept { return base::end();}
    reverse_iterator rbegin() noexcept { return std::reverse_iterator(end());}
    const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator(end());}
    reverse_iterator rend() noexcept { return std::reverse_iterator(begin());}
    const_reverse_iterator rend() const noexcept { return std::reverse_iterator(begin());}
    const_reverse_iterator crbegin() const noexcept { return std::reverse_iterator(end());}
    const_reverse_iterator crend() const noexcept { return std::reverse_iterator(begin());}
    reference front() {
        assert(!empty());
        return base::end_.next->asNode()->value;
    }
    const_reference front() const {
        assert(!empty());
        return base::end_.next->asNode()->value;
    }
    reference back() {
        assert(!empty());
        return base::end_.prev->asNode()->value;
    }
    const_reference back() const {
        assert(!empty());
        return base::end_.prev->asNode()->value;
    }
    void pushFront(value_type&& x);
    void pushBack(value_type&& x);

    template <typename... Args>
    reference emplaceFront(Args&&... args);
    template <typename... Args>
    reference emplaceBack(Args&&... args);

    template <typename... Args>
    iterator emplace(const_iterator p, Args&&... args);

    iterator insert(const_iterator p, value_type&& x);

    iterator insert(const_iterator p, std::initializer_list<value_type> il) {
        return insert(p, il.begin(), il.end());
    }

    void pushFront(const value_type& x);
    void pushBack(const value_type& x);

    template <typename Arg>
    void emplaceBack(Arg&& arg) {
        emplaceBack(std::forward<Arg>(arg));
    }

    iterator insert(const_iterator p, const value_type& x);
    iterator insert(const_iterator p, size_type n, const value_type& x);
    template <typename InputIterator>
    iterator insert(const_iterator p, InputIterator f, InputIterator l,
                    typename std::enable_if_t<std::integral_constant<bool,
                            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                                    std::input_iterator_tag>>::value, void> * = 0);

    void clear() noexcept { base::clear();}

    void popFront();
    void popBack();

    iterator erase(const_iterator p);
    iterator erase(const_iterator f, const_iterator l);

    void resize(size_type n);
    void resize(size_type n, const value_type& x);

    void splice(const_iterator p, List& c);
    void splice(const_iterator p, List&& c) {
        splice(p, c);
    }
    void splice(const_iterator p, List&& c, const_iterator i) {
        splice(p, c, i);
    }
    void splice(const_iterator p, List&& c, const_iterator f, const_iterator l) {
        splice(p, c, f, l);
    }
    void splice(const_iterator p, List& c, const_iterator i);
    void splice(const_iterator p, List& c, const_iterator f, const_iterator l);

    RemoveReturnType remove(const value_type& x);
    template <typename Pred>
    RemoveReturnType removeIf(Pred pred);
    RemoveReturnType unique() {return unique(std::equal_to<void>());}
    template <typename BinaryPred>
    RemoveReturnType unique(BinaryPred binaryPred);
    void merge(List& c);
    void merge(List&& c) {merge(c);}

    template <typename Comp>
    void merge(List&& c, Comp comp) {merge(c, comp);}

    template <typename Comp>
    void merge(List& c, Comp comp);

    void sort();

    template <typename Comp>
    void sort(Comp comp);

    void reverse() noexcept;

    bool invariants() const;

    using NodeDestructor = AllocatorDestructor<NodeAllocator>;
    using HoldPointer = UniquePtr<Node, NodeDestructor>;

    HoldPointer allocateNode(NodeAllocator& na) {
        NodePointer p = NodeAllocTraits::allocate(na, 1);
        p->prev = nullptr;
        return HoldPointer(p, NodeDestructor(na, 1));
    }

private:
    static void linkNodes (LinkPointer p, LinkPointer f, LinkPointer l);
    void linkNodesAtFront (LinkPointer f, LinkPointer l);
    void linkNodesAtBack (LinkPointer f, LinkPointer l);
    iterator Iterator(size_type n);
    template <typename Comp>
    static iterator Sort(iterator f1, iterator e2, size_type n, Comp& comp);

    void moveAssign(List& c, std::true_type) noexcept(std::is_nothrow_move_assignable_v<NodeAllocator>);
    void moveAssign(List& c, std::false_type);

};

template<typename InputIterator,
        typename Alloc = typename std::allocator<typename std::iterator_traits<InputIterator>::value_type>,
        typename = typename std::enable_if_t<std::true_type::value, void>>
List(InputIterator, InputIterator) -> List<typename std::iterator_traits<InputIterator>::value_type, Alloc>;

template<typename InputIterator,
        typename Alloc,
        typename = typename std::enable_if_t<std::true_type::value, void>>
List(InputIterator, InputIterator, Alloc) -> List<typename std::iterator_traits<InputIterator>::value_type, Alloc>;

template <typename T, typename Alloc>
inline void List<T, Alloc>::linkNodes(LinkPointer p, LinkPointer f, LinkPointer l) {
    p->prev->next = f;
    f->prev = p->prev;
    p->prev = l;
    l->next = p;
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::linkNodesAtFront(LinkPointer f, LinkPointer l) {
    f->prev = base::endAsLink();
    l->next = base::end_.next;
    l->next->prev = l;
    base::end_.next = f;
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::linkNodesAtBack(LinkPointer f, LinkPointer l) {
    l->next = base::endAsLink();
    f->prev = base::end_.prev;
    f->prev->next = f;
    base::end_.prev = l;
}

template <typename T, typename Alloc>
inline typename List<T, Alloc>::iterator List<T, Alloc>::Iterator(size_type n) {
    return n <= base::sz() / 2 ? std::next(begin(), n) : std::prev(end(), base::sz() - n);
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type n) {
    for (; n > 0; --n) {
        emplaceBack();
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type n, const allocator_type& a) : base(a) {
    for (; n > 0; --n) {
        emplaceBack();
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type n, const value_type& x) {
    for (; n > 0; --n) {
        pushBack(x);
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type n, const value_type& x, const allocator_type& a) : base(a) {
    for (; n > 0; --n) {
        pushBack(x);
    }
}

template <typename T, typename Alloc>
template <typename InputIterator>
List<T, Alloc>::List(InputIterator f, InputIterator l,
                     typename std::enable_if_t<std::integral_constant<bool,
                             std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                             std::input_iterator_tag> >::value, void> *) {
    for (; f != l; ++f) {
        emplaceBack(*f);
    }
}

template <typename T, typename Alloc>
template <typename InputIterator>
List<T, Alloc>::List(InputIterator f, InputIterator l, const allocator_type& a,
                     typename std::enable_if_t<std::integral_constant<bool,
                             std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                                     std::input_iterator_tag> >::value, void> *) : base(a) {
    for (; f != l; ++f) {
        emplaceBack(*f);
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(const List& c)
: base(NodeAllocTraits::select_on_container_copy_construction(c.nodeAlloc())) {
    for (const_iterator i = c.begin(), e = c.end(); i != e; ++i) {
        push_back(*i);
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(const List& c, const allocator_type& a)
        : base(a) {
    for (const_iterator i = c.begin(), e = c.end(); i != e; ++i) {
        push_back(*i);
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(std::initializer_list<value_type> il, const allocator_type& a) : base(a) {
    for (typename std::initializer_list<value_type>::const_iterator i = il.begin(), e = il.end(); i != e; ++i) {
        push_back(*i);
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(std::initializer_list<value_type> il) {
    for (typename std::initializer_list<value_type>::const_iterator i = il.begin(), e = il.end(); i != e; ++i) {
        push_back(*i);
    }
}

template <typename T, typename Alloc>
inline List<T, Alloc>::List(List&& c) noexcept(std::is_nothrow_move_constructible_v<NodeAllocator>)
: base(std::move(c.nodeAlloc())){
    splice(end(), c);
}

template <typename T, typename Alloc>
inline List<T, Alloc>::List(List&& c, const allocator_type &a) : base (a){
    if (a == c.getAllocator()) {
        splice(end(), c);
    } else {
        assign(std::move_iterator<iterator>(c.begin()), std::move_iterator<iterator>(c.end()));
    }
}

template <typename T, typename Alloc>
inline List<T, Alloc>& List<T, Alloc>::operator=(List&& c) noexcept(NodeAllocTraits::propagate_on_container_move_assignment::value
            && std::is_nothrow_move_assignable_v<NodeAllocator>) {
    moveAssign(c, std::integral_constant<bool, NodeAllocTraits::propagate_on_container_move_assignment::value>());
    return *this;
}

template <typename T, typename Alloc>
void List<T, Alloc>::moveAssign(List& c, std::false_type) {
    if (base::nodeAlloc() != c.nodeAlloc()) {
        assign(std::move_iterator<iterator>(c.begin()), std::move_iterator<iterator>(c.end()));
    } else {
        moveAssign(c, std::true_type());
    }
}

template <typename T, typename Alloc>
void List<T, Alloc>::moveAssign(List& c, std::true_type) noexcept(std::is_nothrow_move_assignable_v<NodeAllocator>) {
    clear();
    base::moveAssignAlloc(c);
    splice(end(), c);
}

template <typename T, typename Alloc>
inline List<T, Alloc>& List<T, Alloc>::operator=(const List &c) {
    if (this != &c) {
        base::copyAssignAlloc(c);
        assign(c.begin(), c.end());
    }
    return *this;
}

template <typename T, typename Alloc>
template <typename InputIterator>
void List<T, Alloc>::assign(InputIterator f, InputIterator l,
                            typename std::enable_if_t<std::integral_constant<bool,
                            std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                            std::input_iterator_tag> >::value, void> *) {
    iterator i = begin();
    iterator e = end();
    for (; f != l && i != e; ++f, ++i){
        *i = *f;
    }
    if (i == e) {
        insert(e, f, l);
    } else {
        erase(i, e);
    }
}

template <typename T, typename Alloc>
inline Alloc List<T, Alloc>::getAllocator() const noexcept {
    return allocator_type(base::nodeAlloc());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::insert(const_iterator p, const value_type& x) {
    NodeAllocator& na = base::nodeAlloc();
    HoldPointer hold = allocateNode(na);
    NodeAllocTraits::construct(na, std::addressof(hold->value), x);
    linkNodes(p.ptr, hold->asLink(), hold->asLink());
    ++base::sz();
    return iterator(hold.release()->asLink());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::insert(const_iterator p, size_type n, const value_type& x) {
    iterator r (p.ptr);
    if (n > 0) {
        size_type ds = 0;
        NodeAllocator& na = base::nodeAlloc();
        HoldPointer hold = allocateNode(na);
        NodeAllocTraits::construct(na, std::addressof(hold->value), x);
        ++ds;
        r = iterator(hold->asLink());
        hold.release();
        iterator e = r;
        try {
            for (--n; n != 0; --n, ++e, ++ds) {
                hold.reset(NodeAllocTraits::allcoate(na, 1));
                NodeAllocTraits::construct(na, std::addressof(hold->value), x);
                e.ptr->next = hold->asLink();
                hold->prev = e.ptr;
                hold.release();
            }
        } catch (...) {
            while (true) {
                NodeAllocTraits::destroy(na, std::addressof(*e));
                LinkPointer prev = e.ptr->prev;
                NodeAllocTraits::deallocate(na, e.ptr->asNode(), 1);
                if (prev == 0) {
                    break;
                }
                e = iterator(prev);
            }
            throw;
        }
        linkNodes(p.ptr, r.ptr, e.ptr);
        base::sz() += ds;
    }
    return r;
}

template <typename T, typename Alloc>
template <typename InputIterator>
typename List<T, Alloc>::iterator
        List<T, Alloc>::insert(const_iterator p, InputIterator f, InputIterator l,
                typename std::enable_if_t<std::integral_constant<bool,
                        std::is_convertible_v<typename std::iterator_traits<InputIterator>::iterator_category,
                        std::input_iterator_tag> >::value, void> *) {
    iterator r(p.ptr);
    if (f != l) {
        size_type ds = 0;
        NodeAllocator& na = base::nodeAlloc();
        HoldPointer hold = allocateNode(na);
        NodeAllocTraits::construct(na, std::addressof(hold->value), *f);
        ++ds;
        r = iterator(hold.get()->asLink());
        hold.release();
        iterator e = r;
        try {
            for (++f; f != l; ++f, (void) ++e, (void) ++ds) {
                hold.reset(NodeAllocTraits::allcoate(na, 1));
                NodeAllocTraits::construct(na, std::addressof(hold->value), *f);
                e.ptr->next = hold.get()->asLink();
                hold->prev = e.ptr;
                hold.release();
            }
        } catch (...) {
            while (true) {
                NodeAllocTraits::destroy(na, std::addressof(*e));
                LinkPointer prev = e.ptr->prev;
                NodeAllocTraits::deallocate(na, e.ptr->asNode(), 1);
                if (prev == 0) {
                    break;
                }
                e = iterator(prev);
            }
            throw;
        }
        linkNodes(p.ptr, r.ptr, e.ptr);
        base::sz() += ds;
    }
    return r;
}

template <typename T, typename Alloc>
void List<T, Alloc>::pushFront(const value_type& x) {
    NodeAllocator& na = base::nodeAlloc();
    HoldPointer hold = allocateNode(na);
    NodeAllocTraits::construct(na, std::addressof(hold->value), x);
    LinkPointer nl = hold->asLink();
    linkNodesAtFront(nl, nl);
    ++base::sz();
    hold.release();
}

template <typename T, typename Alloc>
void List<T, Alloc>::pushBack(const value_type& x) {
    NodeAllocator& na = base::nodeAlloc();
    HoldPointer hold = allocateNode(na);
    NodeAllocTraits::construct(na, std::addressof(hold->value), x);
    linkNodesAtBack(hold.get()->asLink(), hold.get()->asLink());
    ++base::sz();
    hold.release();
}

template <typename T, typename Alloc>
void List<T, Alloc>::pushFront(value_type&& x) {
    NodeAllocator& na = base::nodeAlloc();
    HoldPointer hold = allocateNode(na);
    NodeAllocTraits::construct(na, std::addressof(hold->value), std::move(x));
    linkNodesAtFront(hold.get()->asLink(), hold.get()->asLink());
    ++base::sz();
    hold.release();
}

template <typename T, typename Alloc>
void List<T, Alloc>::pushBack(value_type&& x) {
    NodeAllocator& na = base::nodeAlloc();
    HoldPointer hold = allocateNode(na);
    NodeAllocTraits::construct(na, std::addressof(hold->value), std::move(x));
    linkNodesAtBack(hold.get()->asLink(), hold.get()->asLink());
    ++base::sz();
    hold.release();
}

template <typename T, typename Alloc>
template <typename... Args>
typename List<T, Alloc>::reference List<T, Alloc>::emplaceFront(Args&&... args) {
    NodeAllocator& na = base::nodeAlloc();
    HoldPointer hold = allocateNode(na);
    NodeAllocTraits::construct(na, std::addressof(hold->value), std::forward<Args>(args)...);
    linkNodesAtFront(hold.get()->asLink(), hold.get()->asLink());
    ++base::sz();
    return hold.release()->value;
}

template <typename T, typename Alloc>
template <typename... Args>
typename List<T, Alloc>::reference List<T, Alloc>::emplaceBack(Args&&... args) {
    NodeAllocator& na = base::nodeAlloc();
    HoldPointer hold = allocateNode(na);
    NodeAllocTraits::construct(na, std::addressof(hold->value), std::forward<Args>(args)...);
    LinkPointer nl = hold->asLink();
    linkNodesAtBack(nl, nl);
    ++base::sz();
    return hold.release()->value;
}

template <typename T, typename Alloc>
template <typename... Args>
typename List<T, Alloc>::iterator List<T, Alloc>::emplace(const_iterator p, Args&&... args) {
    NodeAllocator& na = base::nodeAlloc();
    HoldPointer hold = allocateNode(na);
    NodeAllocTraits::construct(na, std::addressof(hold->value), std::forward<Args>(args)...);
    LinkPointer nl = hold.get()->asLink();
    linkNodes(p.ptr, nl, nl);
    ++base::sz();
    hold.release();
    return iterator(nl);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::insert(const_iterator p, value_type&& x) {
    NodeAllocator& na = base::nodeAlloc();
    HoldPointer hold = allocateNode(na);
    NodeAllocTraits::construct(na, std::addressof(hold->value), std::move(x));
    LinkPointer nl = hold.get()->asLink();
    linkNodes(p.ptr, nl, nl);
    ++base::sz();
    hold.release();
    return iterator(nl);
}

template <typename T, typename Alloc>
void List<T, Alloc>::popFront() {
    assert(!empty());
    NodeAllocator& na = base::nodeAlloc();
    LinkPointer n = base::end_.next;
    base::unlinkNodes(n, n);
    base::sz();
    NodePointer np = n->asNode();
    NodeAllocTraits::destroy(na, std::addressof(np->value));
    NodeAllocTraits::deallocate(na, np, 1);
}

template <typename T, typename Alloc>
void List<T, Alloc>::popBack() {
    assert(!empty());
    NodeAllocator& na = base::nodeAlloc();
    LinkPointer n = base::end_.prev;
    base::unlinkNodes(n, n);
    base::sz();
    NodePointer np = n->asNode();
    NodeAllocTraits::destroy(na, std::addressof(np->value));
    NodeAllocTraits::deallocate(na, np, 1);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::erase(const_iterator p) {
    assert(p != end());
    NodeAllocator& na = base::nodeAlloc();
    LinkPointer n = p.ptr;
    LinkPointer r = n->next;
    base::unlinkNodes(n, n);
    --base::sz();
    NodePointer np = n->asNode();
    NodeAllocTraits::destroy(na, std::addressof(np->value));
    NodeAllocTraits::deallocate(na, np, 1);
    return iterator(r);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::erase(const_iterator f, const_iterator l) {
    if (f != l) {
        NodeAllocator& na = base::nodeAlloc();
        base::unlinkNodes(f.ptr, l.ptr->prev);
        while (f != l) {
            LinkPointer n = f.ptr;
            ++f;
            --base::sz();
            NodePointer np = n->asNode();
            NodeAllocTraits::destroy(na, std::addressof(np->value));
            NodeAllocTraits::deallocate(na, np, 1);
        }
    }
    return iterator(l.ptr);
}

template <typename T, typename Alloc>
void List<T, Alloc>::resize(size_type n) {
    if (n < base::sz()) {
        erase(iterator(n), end());
    } else if (n > base::sz()) {
        n -= base::sz();
        size_type ds = 0;
        NodeAllocator& na = base::nodeAlloc();
        HoldPointer hold = allocateNode(na);
        NodeAllocTraits::construct(na, std::addressof(hold->value));
        ++ds;
        iterator r = iterator(hold.release()->asLink());
        iterator e = r;
        try {
            for (--n; n != 0; --n, ++e, ++ds) {
                hold.reset(NodeAllocTraits::allcoate(na, 1));
                NodeAllocTraits::construct(na, std::addressof(hold->value));
                e.ptr->next = hold.get()->asLink();
                hold->prev = e.ptr;
                hold.release();
            }
        } catch (...) {
            while (true) {
                NodeAllocTraits::destroy(na, std::addressof(*e));
                LinkPointer prev = e.ptr->prev;
                NodeAllocTraits::deallocate(na, e.ptr->asNode(), 1);
                if (prev == 0) {
                    break;
                }
                e = iterator(prev);
            }
            throw;
        }
        linkNodesAtBack(r.ptr, e.ptr);
        base::sz() += ds;
    }
}

template <typename T, typename Alloc>
void List<T, Alloc>::resize(size_type n, const value_type& x) {
    if (n < base::sz()) {
        erase(iterator(n), end());
    } else if (n > base::sz()) {
        n -= base::sz();
        size_type ds = 0;
        NodeAllocator& na = base::nodeAlloc();
        HoldPointer hold = allocateNode(na);
        NodeAllocTraits::construct(na, std::addressof(hold->value), x);
        ++ds;
        LinkPointer nl = hold.release()->asLink();
        iterator r = iterator(nl);
        iterator e = r;
        try {
            for (--n; n != 0; --n, ++e, ++ds) {
                hold.reset(NodeAllocTraits::allcoate(na, 1));
                NodeAllocTraits::construct(na, std::addressof(hold->value), x);
                e.ptr->next = hold.get()->asLink();
                hold->prev = e.ptr;
                hold.release();
            }
        } catch (...) {
            while (true) {
                NodeAllocTraits::destroy(na, std::addressof(*e));
                LinkPointer prev = e.ptr->prev;
                NodeAllocTraits::deallocate(na, e.ptr->asNode(), 1);
                if (prev == 0) {
                    break;
                }
                e = iterator(prev);
            }
            throw;
        }
        linkNodes(base::endAsLink(), r.ptr, e.ptr);
        base::sz() += ds;
    }
}

template <typename T, typename Alloc>
void List<T, Alloc>::splice(const_iterator p, List& c) {
    assert(this != &c);
    if (!c.empty()) {
        LinkPointer f = c.end_.next;
        LinkPointer l = c.end_.prev;
        base::unlinkNodes(f, l);
        linkNodes(p.ptr, f, l);
        base::sz() += c.sz();
        c.sz() = 0;
    }
}

template <typename T, typename Alloc>
void List<T, Alloc>::splice(const_iterator p, List& c, const_iterator i) {
    if (p.ptr != i.ptr && p.ptr != i.ptr->next) {
        LinkPointer f = i.ptr;
        base::unlinkNodes(f, f);
        linkNodes(p.ptr, f, f);
        --c.sz();
        ++base::sz();
    }
}

template <typename T, typename Alloc>
void List<T, Alloc>::splice(const_iterator p, List& c, const_iterator f, const_iterator l) {
    if (f != l) {
        LinkPointer first = f.ptr;
        --l;
        LinkPointer last = l.ptr;
        if (this != &c) {
            size_type s = std::distance(f, l) + 1;
            c.sz() -= s;
            base::sz() += s;
        }
        base::unlinkNodes(first, last);
        link_nodes(p.ptr, first, last);
    }
}

template <typename T, typename Alloc>
typename List<T, Alloc>::RemoveReturnType List<T, Alloc>::remove(const value_type& x) {
    List<T, Alloc> deletedNodes(getAllocator());
    for (const_iterator i = begin(), e = end(); i != e;) {
        if (*i == x) {
            const_iterator j = std::next(i);
            for (; j != e && *j == x; ++j) ;
            deletedNodes.splice(deletedNodes.end(), *this, i, j);
            i = j;
            if (i != e) ++i;
        } else {
            ++i;
        }
    }

    return (RemoveReturnType) deletedNodes.size();
}

template <typename T, typename Alloc>
template <typename Pred>
typename List<T, Alloc>::RemoveReturnType List<T, Alloc>::removeIf(Pred pred) {
    List<T, Alloc> deletedNodes(getAllocator());
    for (iterator i = begin(), e = end(); i != e;) {
        if (pred(*i)) {
            iterator j = std::next(i);
            for (; j != e && pred(*j); ++j) ;
            deletedNodes.splice(deletedNodes.end(), *this, i, j);
            i = j;
            if (i != e) ++i;
        } else {
            ++i;
        }
    }

    return (RemoveReturnType) deletedNodes.size();
}

template <typename T, typename Alloc>
template <typename BinaryPred>
typename List<T, Alloc>::RemoveReturnType List<T, Alloc>::unique(BinaryPred binaryPred) {
    List<T, Alloc> deletedNodes(getAllocator());
    for (iterator i = begin(), e = end(); i != e;) {
        iterator j = std::next(i);
        for (; j != e && binaryPred(*i, *j); ++j) ;
        if (++i != j) {
            deletedNodes.splice(deletedNodes.end(), *this, i, j);
            i = j;
        }
    }

    return (RemoveReturnType) deletedNodes.size();
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::merge(List& c) {
    merge(c, std::less<value_type>());
}

template <typename T, typename Alloc>
template <typename Comp>
void List<T, Alloc>::merge(List& c, Comp comp) {
    if (this != std::addressof(c)) {
        iterator f1 = begin();
        iterator e1 = end();
        iterator f2 = c.begin();
        iterator e2 = c.end();
        while (f1 != e1 && f2 != e2) {
            if (comp(*f2, *f1)) {
                size_type ds = 1;
                iterator m2 = std::next(f2);
                for (; m2 != e2 && comp(*m2, *f1); ++m2, ++ds) ;
                base::sz() += ds;
                c.sz() -= ds;
                LinkPointer f = f2.ptr;
                LinkPointer l = m2.ptr->prev;
                f2 = m2;
                base::unlinkNodes(f, l);
                m2 = std::next(f1);
                linkNodes(f1.ptr, f, l);
                f1 = m2;
            } else {
                ++f1;
            }
        }
        splice(e1, c);
    }
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::sort() {
    sort(std::less<value_type>());
}

template <typename T, typename Alloc>
template <typename Comp>
inline void List<T, Alloc>::sort(Comp comp) {
    Sort(begin(), end(), base::sz(), comp);
}

template <typename T, typename Alloc>
template <typename Comp>
typename List<T, Alloc>::iterator List<T, Alloc>::Sort(iterator f1, iterator e2, size_type n, Comp& comp) {
    switch (n) {
        case 0:
        case 1:
            return f1;
        case 2:
            if (comp(*--e2, *f1)) {
                LinkPointer f = e2.ptr;
                base::unlinkNodes(f, f);
                linkNodes(f1.ptr, f, f);
                return e2;
            }
            return f1;
    }
    size_type n2 = n / 2;
    iterator e1 = std::next(f1, n2);
    iterator r = f1 = Sort(f1, e1, n2, comp);
    iterator f2 = e1 = Sort(e1, e2, n - n2, comp);
    if (comp(*f2, *f1)) {
        iterator m2 = std::next(f2);
        for (; m2 != e2 && _comp(*m2, *f1); ++m2) ;
        LinkPointer f = f2.ptr;
        LinkPointer l = m2.ptr->prev;
        r = f2;
        e1 = f2 = m2;
        base::unlinkNodes(f, l);
        m2 = std::next(f1);
        linkNodes(f1.ptr, f, l);
        f1 = m2;
    } else {
        ++f1;
    }
    while (f1 != e1 && f2 != e2) {
        if (comp(*f2, *f1)) {
            iterator m2 = std::next(f2);
            for (; m2 != e2 && _comp(*m2, *f1); ++m2) ;
            LinkPointer f = f2.ptr;
            LinkPointer l = m2.ptr->prev;
            if (e1 == f2) {
                e1 = m2;
            }
            f2 = m2;
            base::unlinkNodes(f, l);
            m2 = std::next(f1);
            linkNodes(f1.ptr, f, l);
            f1 = m2;
        } else {
            ++f1;
        }
    }
    return r;
}

template <typename T, typename Alloc>
void List<T, Alloc>::reverse() noexcept {
    if (base::sz() > 1) {
        iterator e = end();
        for (iterator i = begin(); i.ptr != e.ptr;) {
            std::swap(i.ptr->prev, i.ptr->next);
            i.ptr = i.ptr->prev;
        }
        std::swap(e.ptr->prev, e.ptr->next);
    }
}

template <typename T, typename Alloc>
bool List<T, Alloc>::invariants() const {
    return size() == std::distance(begin(), end());
}

template <typename T, typename Alloc>
inline bool operator==(const List<T, Alloc>& x, const List<T, Alloc>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <typename T, typename Alloc>
inline bool operator<(const List<T, Alloc>& x, const List<T, Alloc>& y) {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, typename Alloc>
inline bool operator!=(const List<T, Alloc>& x, const List<T, Alloc>& y) {
    return !(x == y);
}

template <typename T, typename Alloc>
inline bool operator>(const List<T, Alloc>& x, const List<T, Alloc>& y) {
    return y < x;
}

template <typename T, typename Alloc>
inline bool operator>=(const List<T, Alloc>& x, const List<T, Alloc>& y) {
    return !(x < y);
}

template <typename T, typename Alloc>
inline bool operator<=(const List<T, Alloc>& x, const List<T, Alloc>& y) {
    return !(y < x);
}

template <typename T, typename Allocator, typename Predicate>
inline void eraseIf(List<T, Allocator>& c, Predicate pred) {
    c.removeIf(pred);
}

template <typename T, typename Allocator, typename U>
inline void erase(List<T, Allocator>& c, const U& v) {
    eraseIf(c, [&](auto& elem) {return elem == v;});
}


int main() {

}