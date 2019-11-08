#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

int get_squares(int grains) {
    int j = 0;
    int i = 0;
    while (j < grains) {
        j += std::pow(2, i++);
    }
    return i;
}

int main() {
    int grains = 1'000;
    std::cout << get_squares(grains) << " squares is need to give " << grains << " grains.\n";
    grains = 1'000'000;
    std::cout << get_squares(grains) << " squares is need to give " << grains << " grains.\n";
    grains = 1'000'000'000;
    std::cout << get_squares(grains) << " squares is need to give " << grains << " grains.\n";
}