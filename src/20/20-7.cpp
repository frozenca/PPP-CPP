#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

int main() {
    std::vector<std::string> v {"aa", "Ab", "a", "bbc"};
    auto laststr = *std::max_element(v.begin(), v.end());
    std::cout << laststr;

}