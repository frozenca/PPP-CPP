#include <iostream>
#include <vector>
#include <algorithm>

template <typename T>
void reverseVec(std::vector<T>& v) {
    std::reverse(v.begin(), v.end());
}

template <typename T>
void printVec(const std::vector<T>& v) {
    for (const auto& n : v) {
        std::cout << n << ' ';
    }
    std::cout << '\n';
}

int main() {
    std::vector<int> v1 {1, 3, 5, 7, 9};
    std::vector<std::string> v2 {"1", "3", "5", "7", "9"};
    reverseVec(v1);
    reverseVec(v2);
    printVec(v1);
    printVec(v2);

}