#include <iostream>
#include <fstream>
#include <map>

int main() {
    std::ifstream ifs {"input.txt"};
    if (!ifs) {
        throw std::runtime_error("Input file fail");
    }
    std::map <int, size_t> m;
    for (int n; ifs >> n; ) {
        m[n]++;
    }
    for (const auto& [n, count] : m) {
        std::cout << n << ' ' << count << '\n';
    }


}