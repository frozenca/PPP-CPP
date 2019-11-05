#include <iostream>

int main() {
    std::cout << "Write an integer value:\n";
    int a = 0;
    std::cin >> a;
    if (a % 2) {
        std::cout << a << " is odd\n";
    } else {
        std::cout << a << " is even\n";
    }
}