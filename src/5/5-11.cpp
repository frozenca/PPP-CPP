#include <iostream>

constexpr int fibonacci(int n) {
    if (n < 0) throw std::invalid_argument("n is negative!");
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    int k = 1;

    int twoprev = 1;
    int prev = 1;
    int curr = 2;
    std::cout << 1 << ' ' << 1 << ' ';
    // WARNING: behavior of signed integer overflow is implementation-defined
    while (curr >= prev) {
        twoprev = prev;
        prev = curr;
        curr = prev + curr;
        std::cout << curr << ' ';
    }


}