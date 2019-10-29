#include <iostream>

int main() {
    std::cout << "Write three integer values:\n";
    int a = 0, b = 0, c = 0;
    std::cin >> a >> b >> c;
    if (a <= b) {
        if (b <= c) {
            std::cout << a << ", " << b << ", " << c;
        } else if (a <= c) {
            std::cout << a << ", " << c << ", " << b;
        } else {
            std::cout << c << ", " << a << ", " << b;
        }
    } else if (b <= c) {
        std::cout << b << ", " << a << ", " << c;
    } else if (a <= c) {
        std::cout << b << ", " << c << ", " << a;
    } else {
        std::cout << c << ", " << b << ", " << a;
    }
}