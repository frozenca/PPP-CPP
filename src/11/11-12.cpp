#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main() {
    std::ifstream ifs {"input.txt"};
    if (!ifs) {
        throw std::runtime_error("Input file failed");
    }
    std::vector<char> v;
    for (char c; ifs.get(c);) {
        v.push_back(c);
    }
    std::reverse(v.begin(), v.end());
    for (char c : v) {
        std::cout << c;
    }


}