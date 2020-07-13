#include <iterator>
#include <type_traits>

template <typename Compare, typename ForwardIterator, typename T>
constexpr ForwardIterator LowerBound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    using difference_type = typename std::iterator_traits<ForwardIterator>::difference_type;
    difference_type len = std::distance(first, last);
    while (len != 0) {
        difference_type l2 = static_cast<difference_type>(static_cast<typename std::make_unsigned_t<difference_type>>(len) / 2);
        ForwardIterator m = first;
        std::advance(m, l2);
        if (comp(*m, value)) {
            first = ++m;
            len -= l2 + 1;
        } else {
            len = l2;
        }
    }
    return first;
}

template <typename ForwardIterator, typename T, typename Compare>
[[nodiscard]] inline constexpr ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    using CompRef = typename std::add_lvalue_reference_t<Compare>;
    return LowerBound<CompRef>(first, last, value, comp);
}

template <typename ForwardIterator, typename T>
[[nodiscard]] inline constexpr ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value) {
    return lower_bound(first, last, value, std::less<T>());
}

template <typename Compare, typename ForwardIterator, typename T>
inline constexpr bool BinarySearch(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    first = lower_bound<Compare>(first, last, value, comp);
    return first != last && !comp(value, *first);
}

template <typename ForwardIterator, typename T, typename Compare>
[[nodiscard]] inline constexpr bool binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    using CompRef = typename std::add_lvalue_reference_t<Compare>;
    return BinarySearch<CompRef>(first, last, value, comp);
}

template <typename ForwardIterator, typename T>
[[nodiscard]] inline constexpr bool binary_search(ForwardIterator first, ForwardIterator last, const T& value) {
    return binary_search(first, last, value, std::less<T>());
}

int main() {

}