#include <algorithm>
#include <cassert>
#include <ranges>
#include <vector>

int main() {
    std::vector<int> v {1, 2, 3, 5, 8, 13, 21};
    assert(std::ranges::binary_search(v, 1));
    assert(std::ranges::binary_search(v, 2));
    assert(std::ranges::binary_search(v, 3));
    assert(!std::ranges::binary_search(v, 4));
    assert(std::ranges::binary_search(v, 5));
    assert(std::ranges::binary_search(v, 8));
    assert(std::ranges::binary_search(v, 13));
    assert(std::ranges::binary_search(v, 21));


}