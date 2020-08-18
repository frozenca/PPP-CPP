#include <cstddef>
#include <iostream>
#include <iomanip>

int main() {
    for (int i = 0; i <= 400; i++) {
        std::cout << std::hex << i << '\n';
    }
    for (int i = -200; i <= 200; i++) {
        std::cout << std::hex << i << '\n';
    }

}