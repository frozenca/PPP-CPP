#include <algorithm>
#include <cassert>
#include <functional>
#include <ranges>
#include <vector>

int main() {
    std::vector<int> v {21, 13, 8, 5, 3, 2, 1};
    assert(std::ranges::binary_search(v, 1, std::greater<>()));
    assert(std::ranges::binary_search(v, 2, std::greater<>()));
    assert(std::ranges::binary_search(v, 3, std::greater<>()));
    assert(!std::ranges::binary_search(v, 4, std::greater<>()));
    assert(std::ranges::binary_search(v, 5, std::greater<>()));
    assert(std::ranges::binary_search(v, 8, std::greater<>()));
    assert(std::ranges::binary_search(v, 13, std::greater<>()));
    assert(std::ranges::binary_search(v, 21, std::greater<>()));


}