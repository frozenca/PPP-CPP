#include <algorithm>
#include <cassert>
#include <ranges>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> v {"a", "b", "c", "d", "f", "g", "i"};
    assert(std::ranges::binary_search(v, "a"));
    assert(std::ranges::binary_search(v, "b"));
    assert(std::ranges::binary_search(v, "c"));
    assert(!std::ranges::binary_search(v, "D"));
    assert(std::ranges::binary_search(v, "f"));
    assert(std::ranges::binary_search(v, "g"));
    assert(std::ranges::binary_search(v, "i"));
    assert(!std::ranges::binary_search(v, "I"));

    std::vector<double> v2 {1.0, 2.0, 3.0, 5.0, 8.0, 13.0, 21.0};
    assert(std::ranges::binary_search(v2, 1.0));
    assert(std::ranges::binary_search(v2, 2.0));
    assert(std::ranges::binary_search(v2, 3.0));
    assert(!std::ranges::binary_search(v2, 4.0));
    assert(std::ranges::binary_search(v2, 5.0));

}