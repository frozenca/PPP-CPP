#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <ranges>
#include <string>
#include <utility>
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

    std::uniform_int_distribution<> len_dist(1, 100);
    std::uniform_int_distribution<> char_dist(0, 25);

    std::string chars = "abcdefghijklmnopqrstuvwxyz";

    N = 50'000;

    std::vector<std::string> v2;
    v2.reserve(N);

    for (std::size_t i = 0; i < N; i++) {
        std::string s;
        std::size_t len = len_dist(gen);
        for (std::size_t j = 0; j < len; j++) {
            s.push_back(chars[char_dist(gen)]);
        }
        v2.push_back(std::move(s));
    }

    t1 = std::chrono::steady_clock::now();
    std::ranges::sort(v2);
    t2 = std::chrono::steady_clock::now();
    d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

    N = 500'000;
    v2.clear();
    v2.reserve(N);

    for (std::size_t i = 0; i < N; i++) {
        std::string s;
        std::size_t len = len_dist(gen);
        for (std::size_t j = 0; j < len; j++) {
            s.push_back(chars[char_dist(gen)]);
        }
        v2.push_back(std::move(s));
    }

    t1 = std::chrono::steady_clock::now();
    std::ranges::sort(v2);
    t2 = std::chrono::steady_clock::now();
    d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

}