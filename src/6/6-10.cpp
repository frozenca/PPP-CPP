#include <iostream>

constexpr size_t factorial(size_t n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    size_t a = 0, b = 0;
    std::cin >> a >> b;
    if (b > a || a == 0) {
        std::cout << "Invalid request";
    } else {
        std::cout << "Permutation: " << factorial(a) / factorial(a - b) << '\n';
        std::cout << "Combination: " << factorial(a) / (factorial(a - b) * factorial(b)) << '\n';
    }
}