#include <cassert>
#include <vector>
#include <iostream>

template <typename T>
void f(std::vector<T>& v1, const std::vector<T>& v2) {
    for (size_t i = 0; i < std::min(v1.size(), v2.size()); i++) {
        v1[i] += v2[i];
    }
}

int main() {
    std::vector<float> v1 {1, 2, 3};
    std::vector<float> v2 {4, 5, 6};
    f(v1, v2);
    for (auto n : v1) {
        std::cout << n << ' ';
    }
}