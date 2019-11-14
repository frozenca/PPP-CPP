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

int main() {
    double c = 0.0;
    std::cin >> c;
    double k = ctok(c);
    std::cout << k << '\n';

}