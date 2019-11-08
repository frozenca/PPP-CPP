#include <iostream>

int square(int x) {
    int sq = 0;
    for (int i = 0; i < x; i++) {
        sq += x;
    }
    return sq;
}

int main() {
    std::cout << square(5) << '\n';
    std::cout << square(10) << '\n';
}