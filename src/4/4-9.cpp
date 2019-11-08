#include <algorithm>
#include <cmath>
#include <iostream>

int get_grains(int squares) {
    int j = 0;
    int i = 0;
    while (i < squares) {
        j += std::pow(2, i++);
    }
    return j;
}

double get_grains_double(int squares) {
    double j = 0;
    int i = 0;
    while (i < squares) {
        j += std::pow(2.0, i++);
    }
    return j;
}

int main() {
    int squares = 31;
    std::cout << get_grains(squares) << " grains is need to fill " << squares << " squares.\n";
    squares = 32;
    std::cout << get_grains(squares) << " grains is need to fill " << squares << " squares.. Oops! something is wrong\n";
    squares = 64;
    std::cout << get_grains_double(squares) << " grains is need to fill " << squares << " squares.\n";
}