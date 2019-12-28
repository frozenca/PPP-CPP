#include <iostream>
#include <vector>
#include <string>

std::vector<int> count_letters (const std::vector<std::string>& v) {
    std::vector<int> res;
    res.reserve(v.size());
    for (const auto& s : v) {
        res.push_back(s.size());
    }
    return res;
}

int main() {
    std::vector<std::string> v {"1", "12", "123", "1234"};
    auto res = count_letters(v);
    for (auto n : res) {
        std::cout << n << ' ';
    }
}