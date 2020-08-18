#include <array>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include <memory_resource>

int main() {
    constexpr size_t N = 1'000;
    constexpr size_t SZ = 1'000;
    constexpr size_t trials = 10'000;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> sz(1, SZ);

    std::chrono::duration<int, std::micro> dt5 {0};

    for (size_t t = 0; t < trials; t++) {
        auto t1 = std::chrono::steady_clock::now();
        {
            std::array<std::byte, N * SZ> buf;
            std::pmr::monotonic_buffer_resource pool2 {buf.data(), buf.size()};
        }
        auto t2 = std::chrono::steady_clock::now();
        dt5 += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    }
    std::cout << dt5.count() / trials << "us\n";

}