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
    std::vector<std::string> v;
    for (std::string s; ifs >> s;) {
        v.push_back(s);
    }
    std::reverse(v.begin(), v.end());
    for (const auto& s : v) {
        std::cout << s << ' ';
    }


}