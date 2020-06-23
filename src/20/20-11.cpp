#include <vector>
#include <iostream>
#include <list>
#include <algorithm>

std::vector<double> listToVec(const std::list<int>& l) {
    std::vector<double> v;
    for (auto n : l) {
        v.push_back(n);
    }
    return v;
}

int main() {
    std::list<int> l {2, 3, 1, 5, 4};
    auto v = listToVec(l);
    std::sort(v.begin(), v.end());
    for (auto n : v) {
        std::cout << n << ' ';
    }

}