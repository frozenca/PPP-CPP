#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

int main() {
    std::vector<double> v {1, 4, 9, 16, 25, 36, 49};

    std::cout << "Min : " << *std::min_element(v.begin(), v.end()) << '\n';
    std::cout << "Max : " << *std::max_element(v.begin(), v.end()) << '\n';
    std::cout << "Mean : " << std::accumulate(v.begin(), v.end(), 0.0) / v.size() << '\n';
    std::cout << "Median : " << (v[v.size() / 2] + v[(v.size() - 1) / 2]) / 2 << '\n';
}