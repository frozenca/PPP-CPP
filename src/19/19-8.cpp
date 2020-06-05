#include <cassert>
#include <cstddef>
#include <limits>
#include <stdexcept>

template <typename T>
class Allocator {
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    constexpr Allocator() noexcept {}

    template <typename U>
    Allocator(const Allocator<U>&) noexcept {}

    [[nodiscard]] pointer allocate(size_type n) {
        if (n > std::numeric_limits<size_type>::max() / sizeof(value_type)) {
            throw std::length_error("Allocate request exceeds the maximum size");
        }
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }
    constexpr void deallocate(pointer p, size_type n) noexcept {
        ::operator delete(p);
    }

};

template <typename T, typename U>
constexpr bool operator==(const Allocator<T>& a1, const Allocator<U>& a2) noexcept {
    return true;
}

int main() {
}