#include <iostream>

int main() {
    std::cout << "Please enter distance in miles:\n";
    double d = 0.0;
    std::cin >> d;
    std::cout << "The distance is " << d * 1.609 << " kilometers\n";
}