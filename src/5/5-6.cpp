#include <stdexcept>
#include <iostream>
#include <vector>

double ctof(double c) {
    if (c < -273.15) {
        throw std::runtime_error("invalid temperature");
    }
    double f = c * (9.0 / 5.0) + 32.0;
    return f;
}

double ftoc(double f) {
    double c = (f - 32.0) * (5.0 / 9.0);
    if (c < -273.15) {
        throw std::runtime_error("invalid temperature");
    }
    return c;
}

int main() {
    double c = 0.0;
    std::cin >> c;
    double f = ctof(c);
    std::cout << f << '\n';
    double c2 = ftoc(f);
    std::cout << c2 << '\n';

}