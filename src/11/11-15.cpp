#include <iostream>
#include <fstream>
#include <iomanip>

int main() {
    std::ifstream ifs {"input.txt"};
    if (!ifs) {
        throw std::runtime_error("Input file fail");
    }
    for (double n; ifs >> n; ) {
        std::cout << std::setw(20) << std::scientific << std::setprecision(8) << n << '\n';
    }


}