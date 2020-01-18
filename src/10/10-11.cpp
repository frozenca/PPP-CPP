#include <fstream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

int main() {
    std::ifstream ifs {"input.txt"};
    if (!ifs) {
        throw std::runtime_error("input open failed");
    }
    int sum = 0;
    while (true) {
        for (int temp; ifs >> temp;) {
            sum += temp;
        }
        if (ifs.eof()) break;
        if (ifs.bad()) throw std::runtime_error("Bad stream");
        if (ifs.fail()) {
            ifs.clear();
            std::string s;
            ifs >> s;
        }
    }
    std::cout << "Sum is: " << sum << '\n';

}