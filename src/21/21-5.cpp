#include <iterator>

template <typename InputIterator, typename T>
[[nodiscard]] inline constexpr InputIterator
find(InputIterator first, InputIterator last, const T& value) {
    typename std::iterator_traits<InputIterator>::difference_type r (0);
    for (; first != last; ++first) {
        if (*first == value) {
            break;
        }
    }
    return first;
}

int main() {

}