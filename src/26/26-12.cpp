#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>

int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(0.0, 10000.0);

    std::size_t N = 50'000;

    std::vector<double> v;
    v.reserve(N);

    for (std::size_t i = 0; i < N; i++) {
        v.push_back(dist(gen));
    }
    auto t1 = std::chrono::steady_clock::now();
    std::ranges::sort(v);
    auto t2 = std::chrono::steady_clock::now();
    auto d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

    N = 500'000;
    v.clear();
    v.reserve(N);

    for (std::size_t i = 0; i < N; i++) {
        v.push_back(dist(gen));
    }
    t1 = std::chrono::steady_clock::now();
    std::ranges::sort(v);
    t2 = std::chrono::steady_clock::now();
    d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

}