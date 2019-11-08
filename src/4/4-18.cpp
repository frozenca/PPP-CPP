#include <cmath>
#include <complex>
#include <iostream>

int main() {
    double a = 0.0, b = 0.0, c = 0.0;
    std::cin >> a >> b >> c;
    std::complex<double> x1 = (-b + std::sqrt(std::complex<double>(b * b - 4 * a * c, 0))) / (2 * a);
    std::complex<double> x2 = (-b - std::sqrt(std::complex<double>(b * b - 4 * a * c, 0))) / (2 * a);

    std::cout << "The solution is: " << x1 << ", " << x2;
}