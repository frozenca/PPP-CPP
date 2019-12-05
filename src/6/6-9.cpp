#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

std::vector<std::string> digits {"one", "ten", "hundred", "thousand"};

int main() {
    std::string str;
    std::cin >> str;

    std::vector<std::string> result;

    size_t j = 0;
    for (size_t i = str.size() - 1; i < str.size(); i--) {
        char c = str[i];
        if (c > '9' || c < '0') throw std::invalid_argument("Not a number");
        int n = c - '0';
        if (j >= 4) break;
        if (j > 0) {
            result.emplace_back("and");
        }
        result.push_back(digits[j++] + ((n > 1) ? "s" : ""));
        result.push_back(std::to_string(n));
    }
    std::reverse(result.begin(), result.end());
    for (const auto& res : result) {
        std::cout << res << ' ';
    }
}