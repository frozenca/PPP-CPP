#include <cstddef>
#include <iostream>
#include <limits>

int main() {
    for (signed char c = 0; c < std::numeric_limits<signed char>::max(); c++) {
        std::cout << static_cast<int>(c) << '\n';
    }

}