#include <stdexcept>
#include <iostream>
#include <cmath>

int main() {
    double a = 0.0, b = 0.0, c = 0.0;
    std::cin >> a >> b >> c;

    double D = b * b - 4 * a * c;
    if (D < 0) {
        std::cout << "Sorry, this equation has no real root";
        return EXIT_FAILURE;
    }
    double x = (-b + std::sqrt(D)) / (2 * a);
    double y = (-b - std::sqrt(D)) / (2 * a);
    std::cout << "x : " << x << " y : " << y << '\n';
}