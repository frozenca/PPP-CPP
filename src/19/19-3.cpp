#include <cassert>
#include <utility>
#include <type_traits>

struct NonType {
    NonType() = delete;

    NonType(const NonType &) = delete;

    NonType &operator=(const NonType &) = delete;

    ~NonType() = delete;
};

template<typename T1, typename T2>
struct Pair {
    T1 first;
    T2 second;

    template<std::enable_if_t<
            std::is_default_constructible_v<T1> && std::is_default_constructible_v<T2>, bool> = false>
    explicit constexpr Pair() noexcept(std::is_nothrow_default_constructible_v<T1>
                                       && std::is_nothrow_default_constructible_v<T2>) : first(), second() {}

    template<std::enable_if_t<
            std::is_constructible_v<T1, T1 const &> &&
            std::is_constructible_v<T1, T2 const &> &&
            std::is_convertible_v<T1 const &, T1> &&
            std::is_convertible_v<T2 const &, T2>, bool> = false>
    constexpr Pair(T1 const &t1, T2 const &t2) noexcept(std::is_nothrow_copy_constructible_v<T1>
                                                        && std::is_nothrow_copy_constructible_v<T2>)
            : first(t1), second(t2) {}

    template<typename U1, typename U2, std::enable_if_t<
            std::is_constructible_v<T1, U1> &&
            std::is_constructible_v<T2, U2> &&
            (!std::is_convertible_v<U1, T1>
             || !std::is_convertible_v<U2, T2>), bool> = false>
    explicit constexpr Pair(U1 &&u1, U2 &&u2) noexcept(std::is_nothrow_constructible_v<T1, U1>
                                                       && std::is_nothrow_constructible_v<T2, U2>)
            : first(std::forward<U1>(u1)), second(std::forward<U2>(u2)) {}

    template<typename U1, typename U2, std::enable_if_t<
            std::is_constructible_v<T1, U1> &&
            std::is_constructible_v<T2, U2> &&
            std::is_convertible_v<U1, T1> &&
            std::is_convertible_v<U2, T2>, bool> = false>
    constexpr Pair(U1 &&u1, U2 &&u2) noexcept(std::is_nothrow_constructible_v<T1, U1>
                                              && std::is_nothrow_constructible_v<T2, U2>)
            : first(std::forward<U1>(u1)), second(std::forward<U2>(u2)) {}

    template<typename U1, typename U2, std::enable_if_t<
            std::is_constructible_v<T1, U1 const&> &&
            std::is_constructible_v<T2, U2 const&> &&
            (!std::is_convertible_v<U1 const&, T1>
             || !std::is_convertible_v<U2 const&, T2>), bool> = false>
    explicit constexpr Pair(Pair<U1, U2> const &p) noexcept(std::is_nothrow_constructible_v<T1, U1>
                                                            && std::is_nothrow_constructible_v<T2, U2>)
            : first(p.first), second(p.second) {}

    template<typename U1, typename U2, std::enable_if_t<
            std::is_constructible_v<T1, U1 const&> &&
            std::is_constructible_v<T2, U2 const&> &&
            std::is_convertible_v<U1 const&, T1> &&
            std::is_convertible_v<U2 const&, T2>, bool> = false>
    constexpr Pair(Pair<U1, U2> const &p) noexcept(std::is_nothrow_constructible_v<T1, U1>
                                                   && std::is_nothrow_constructible_v<T2, U2>)
            : first(p.first), second(p.second) {}

    template<typename U1, typename U2, std::enable_if_t<
            std::is_constructible_v<T1, U1> &&
            std::is_constructible_v<T2, U2> &&
            (!std::is_convertible_v<U1, T1>
             || !std::is_convertible_v<U2, T2>), bool> = false>
    explicit constexpr Pair(Pair<U1, U2> &&p) noexcept(std::is_nothrow_constructible_v<T1, U1>
                                                       && std::is_nothrow_constructible_v<T2, U2>)
            : first(std::forward<U1>(p.first)), second(std::forward<U2>(p.second)) {}

    template<typename U1, typename U2, std::enable_if_t<
            std::is_constructible_v<T1, U1> &&
            std::is_constructible_v<T2, U2> &&
            std::is_convertible_v<U1, T1> &&
            std::is_convertible_v<U2, T2>, bool> = false>
    constexpr Pair(Pair<U1, U2> &&p) noexcept(std::is_nothrow_constructible_v<T1, U1>
                                              && std::is_nothrow_constructible_v<T2, U2>)
            : first(std::forward<U1>(p.first)), second(std::forward<U2>(p.second)) {}

    Pair &operator=(typename std::conditional_t<
            std::is_copy_assignable_v<T1> && std::is_copy_assignable_v<T2>, Pair, NonType> const &p)
    noexcept(std::is_nothrow_copy_assignable_v<T1> && std::is_nothrow_copy_assignable_v<T2>) {
        first = p.first;
        second = p.second;
        return *this;
    }

    Pair &operator=(typename std::conditional_t<
            std::is_move_assignable_v<T1> && std::is_move_assignable_v<T2>, Pair, NonType> &&p)
    noexcept(std::is_nothrow_move_assignable_v<T1> && std::is_nothrow_move_assignable_v<T2>) {
        first = std::forward<T1>(p.first);
        second = std::forward<T2>(p.second);
        return *this;
    }
};

template<typename T1, typename T2>
inline constexpr bool operator==(const Pair<T1, T2> &p1, const Pair<T1, T2> &p2) {
    return p1.first == p2.first && p1.second == p2.second;
}

template<typename T1, typename T2>
inline constexpr bool operator!=(const Pair<T1, T2> &p1, const Pair<T1, T2> &p2) {
    return !(p1 == p2);
}

template<typename T1, typename T2>
inline constexpr bool operator<(const Pair<T1, T2> &p1, const Pair<T1, T2> &p2) {
    return p1.first < p2.first || (!(p2.first < p1.first) && p1.second < p2.second);
}

template<typename T1, typename T2>
inline constexpr bool operator>(const Pair<T1, T2> &p1, const Pair<T1, T2> &p2) {
    return p2 < p1;
}

template<typename T1, typename T2>
inline constexpr bool operator>=(const Pair<T1, T2> &p1, const Pair<T1, T2> &p2) {
    return !(p1 < p2);
}

template<typename T1, typename T2>
inline constexpr bool operator<=(const Pair<T1, T2> &p1, const Pair<T1, T2> &p2) {
    return !(p1 > p2);
}

int main() {
}