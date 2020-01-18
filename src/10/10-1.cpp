#include <fstream>
#include <iostream>
#include <stdexcept>

int main() {
    std::ifstream ifs {"input.txt"};
    if (!ifs) {
        throw std::runtime_error("Can't open input file");
    }
    ifs.exceptions(ifs.exceptions() | std::ios_base::badbit);

    int sum = 0;
    int temp = 0;
    while (ifs >> temp) {
        sum += temp;
    }

    std::cout << "Sum is: " << sum;
}