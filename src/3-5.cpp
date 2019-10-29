#include <iostream>

int main() {
    std::cout << "Please enter two floating-point values:\n";
    double val1 = 0, val2 = 0;
    std::cin >> val1 >> val2;
    std::cout << "smaller: " << std::min(val1, val2) << "\n";
    std::cout << "larger: " << std::max(val1, val2) << "\n";
    std::cout << "sum: " << val1 + val2 << "\n";
    std::cout << "diff: " << val1 - val2 << "\n";
    std::cout << "prod: " << val1 * val2 << "\n";
    std::cout << "ratio: " << val1 / (val2 * 1.0) << "\n";
}