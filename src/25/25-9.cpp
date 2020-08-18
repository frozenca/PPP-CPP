#include <cstddef>
#include <iostream>
#include <limits>

int main() {
    std::cout << sizeof(int) << '\n';
    std::cout << std::numeric_limits<char>::is_signed << '\n';

}