#include <iostream>
#include <vector>

void print(const std::vector<int>& v, std::string& str) {
    for (auto num : v) {
        str += std::to_string(num);
        str += ' ';
    }
}

int main() {
    std::vector<int> v {1, 2, 3, 4};
    std::string str;
    print(v, str);
    std::cout << str;
}