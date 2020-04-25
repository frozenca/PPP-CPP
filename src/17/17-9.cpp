#include <iostream>

int main() {
    int x[3] {0};
    std::cout << &x[0] << ' ' << &x[2] << '\n';

    int* y = new int[3];
    std::cout << &y[0] << ' ' << &y[2] << '\n';
    delete[] y;
}