#include <iterator>

template <typename InputIterator, typename Predicate>
[[nodiscard]] inline constexpr typename std::iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate pred) {
    typename std::iterator_traits<InputIterator>::difference_type r (0);
    for (; first != last; ++first) {
        if (pred(*first)) {
            ++r;
        }
    }
    return r;
}

int main() {

}