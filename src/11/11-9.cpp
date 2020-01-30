#include <fstream>
#include <iostream>
#include <vector>

int main() {
    std::ifstream ifs {"input.txt", std::ios_base::binary};
    if (!ifs) {
        throw std::runtime_error("Can't open input file");
    }

    std::ofstream ofs {"output.txt", std::ios_base::binary};
    if (!ofs) {
        throw std::runtime_error("Can't open output file");
    }
    std::vector<char> v;
    for (char x; ifs.read(&x, 1);) {
        v.push_back(x);
    }
    for (char x : v) {
        ofs.write(reinterpret_cast<char*>(x), 1);
    }
}