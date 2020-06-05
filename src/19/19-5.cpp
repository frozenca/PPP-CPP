#include <cassert>
#include <limits>
#include <stdexcept>
#include <iostream>

struct Int {
    int v;

    Int(int v) : v(v) {}

    Int &operator+=(const Int &rhs) {
        if ((v > 0 && rhs.v > std::numeric_limits<int>::max() - v)
            || (rhs.v < 0 && v < std::numeric_limits<int>::min() - v)) {
            throw std::overflow_error("Arithmetic overflow");
        }
        v += rhs.v;
        return *this;
    }

    friend Int operator+(Int lhs, const Int &rhs) {
        lhs += rhs;
        return lhs;
    }

    Int &operator-=(const Int &rhs) {
        if ((rhs.v < 0 && v > std::numeric_limits<int>::max() + rhs.v)
            || (rhs.v > 0 && v < std::numeric_limits<int>::min() + rhs.v)) {
            throw std::overflow_error("Arithmetic overflow");
        }
        v -= rhs.v;
        return *this;
    }

    friend Int operator-(Int lhs, const Int &rhs) {
        lhs -= rhs;
        return lhs;
    }

    Int &operator*=(const Int &rhs) {
        if (v == 0) {
            return *this;
        }
        if (v > 0 && (rhs.v > std::numeric_limits<int>::max() / v
                      || rhs.v < std::numeric_limits<int>::min() / v)) {
            throw std::overflow_error("Arithmetic overflow");
        }
        if (v < 0 && (rhs.v < std::numeric_limits<int>::max() / v
                      || rhs.v > std::numeric_limits<int>::min() / v)) {
            throw std::overflow_error("Arithmetic overflow");
        }
        v *= rhs.v;
        return *this;
    }

    friend Int operator*(Int lhs, const Int &rhs) {
        lhs *= rhs;
        return lhs;
    }

    Int &operator/=(const Int &rhs) {
        if (rhs.v == 0) {
            throw std::invalid_argument("Division by zero");
        }
        v /= rhs.v;
        return *this;
    }

    friend Int operator/(Int lhs, const Int &rhs) {
        lhs /= rhs;
        return lhs;
    }
};

std::ostream& operator<<(std::ostream& os, const Int& i) {
    os << i.v;
    return os;
}

std::istream& operator>>(std::istream& is, Int& i) {
    int v = 0;
    if (is >> v) {
        i.v = v;
    } else {
        is.setstate(std::ios::failbit);
    }
    return is;
}

int main() {
    Int n = 0;
    std::cin >> n;
    std::cout << n << '\n';
    Int m = 2;
    std::cout << n + m << '\n';
    std::cout << n - m << '\n';
    std::cout << n * m << '\n';
    std::cout << n / m << '\n';
}