#include <cassert>
#include <vector>
#include <numeric>
#include <iostream>

template <typename T>
T f(const std::vector<T>& vt, const std::vector<T>& vu) {
    return std::inner_product(vt.begin(), vt.end(), vu.begin(), T{0});
}

int main() {
    std::vector<float> v1 {1, 2, 3};
    std::vector<float> v2 {4, 5, 6};
    std::cout << f(v1, v2);
}