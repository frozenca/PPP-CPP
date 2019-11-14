#include <stdexcept>
#include <iostream>
#include <vector>

double ctok(double c) {
    double k = c + 273.15;
    if (k < 0.0) {
        throw std::runtime_error("invalid temperature");
    }
    return k;
}

double ktoc(double k) {
    if (k < 0.0) {
        throw std::runtime_error("invalid temperature");
    }
    double c = k - 273.15;
    return c;
}

int main() {
    double c = 0.0;
    std::cin >> c;
    double k = ctok(c);
    std::cout << k << '\n';
    double c2 = ktoc(k);
    std::cout << c2 << '\n';

}