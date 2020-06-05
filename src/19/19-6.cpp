#include <cassert>
#include <limits>
#include <stdexcept>
#include <iostream>
#include <type_traits>

template<typename T>
struct Number {
    T v;

    template<typename U, std::enable_if_t<std::is_constructible_v<T, U> && std::is_arithmetic_v<T>
            && std::is_arithmetic_v<U>, bool> = false>
    Number(U v) : v(v) {
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    Number& operator+=(const Number<U> &rhs) {
        if ((v > 0 && rhs.v > std::numeric_limits<V>::max() - v)
            || (rhs.v < 0 && v < std::numeric_limits<V>::lowest() - v)) {
            throw std::overflow_error("Arithmetic overflow");
        }
        v += rhs.v;
        return *this;
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    friend Number operator+(Number lhs, const Number<U> &rhs) {
        lhs += rhs;
        return lhs;
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    Number& operator-=(const Number<U> &rhs) {
        if ((rhs.v < 0 && v > std::numeric_limits<V>::max() + rhs.v)
            || (rhs.v > 0 && v < std::numeric_limits<V>::lowest() + rhs.v)) {
            throw std::overflow_error("Arithmetic overflow");
        }
        v -= rhs.v;
        return *this;
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    friend Number operator-(Number lhs, const Number<U> &rhs) {
        lhs -= rhs;
        return lhs;
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    Number& operator*=(const Number<U> &rhs) {
        if (v == 0) {
            return *this;
        }
        if (v > 0 && (rhs.v > std::numeric_limits<V>::max() / v
                      || rhs.v < std::numeric_limits<V>::lowest() / v)) {
            throw std::overflow_error("Arithmetic overflow");
        }
        if (v < 0 && (rhs.v < std::numeric_limits<V>::max() / v
                      || rhs.v > std::numeric_limits<V>::lowest() / v)) {
            throw std::overflow_error("Arithmetic overflow");
        }
        v *= rhs.v;
        return *this;
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    friend Number operator*(Number lhs, const Number<U> &rhs) {
        lhs *= rhs;
        return lhs;
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    Number& operator/=(const Number<U> &rhs) {
        if (rhs.v == 0) {
            throw std::invalid_argument("Division by zero");
        }
        v /= rhs.v;
        return *this;
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    friend Number operator/(Number lhs, const Number<U> &rhs) {
        lhs /= rhs;
        return lhs;
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    Number& operator%=(const Number<U> &rhs) {
        if (rhs.v == 0) {
            throw std::invalid_argument("Division by zero");
        }
        v %= rhs.v;
        return *this;
    }

    template<typename U, typename V = std::common_type_t<T, U>, std::enable_if_t<std::is_constructible_v<T, U>, bool> = false>
    friend Number operator%(Number lhs, const Number<U> &rhs) {
        lhs %= rhs;
        return lhs;
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Number<T> &i) {
    os << i.v;
    return os;
}

template <typename T>
std::istream &operator>>(std::istream &is, Number<T> &i) {
    T v = 0;
    if (is >> v) {
        i.v = v;
    } else {
        is.setstate(std::ios::failbit);
    }
    return is;
}

int main() {
    Number<int> n = 0;
    std::cin >> n;
    std::cout << n << '\n';
    Number<int> m = 2;
    std::cout << n + m << '\n';
    std::cout << n - m << '\n';
    std::cout << n * m << '\n';
    std::cout << n / m << '\n';
    std::cout << n % m << '\n';

    Number<double> d = 1.0;
    Number<double> e = 2.3;
    std::cout << d + e << '\n';
    std::cout << d - e << '\n';
    std::cout << d * e << '\n';
    std::cout << d / e << '\n';
//    std::cout << d % e << '\n';
}