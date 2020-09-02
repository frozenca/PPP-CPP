#include <chrono>
#include <iostream>
#include <set>
#include <random>
#include <string>
#include <utility>

int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(0.0, 10000.0);

    std::size_t N = 50'000;

    std::set<double> v;

    auto t1 = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < N; i++) {
        v.insert(dist(gen));
    }
    auto t2 = std::chrono::steady_clock::now();
    auto d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

    N = 500'000;
    v.clear();

    t1 = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < N; i++) {
        v.insert(dist(gen));
    }
    t2 = std::chrono::steady_clock::now();
    d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

    std::uniform_int_distribution<> len_dist(1, 100);
    std::uniform_int_distribution<> char_dist(0, 25);

    std::string chars = "abcdefghijklmnopqrstuvwxyz";

    N = 50'000;

    std::set<std::string> v2;

    t1 = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < N; i++) {
        std::string s;
        std::size_t len = len_dist(gen);
        for (std::size_t j = 0; j < len; j++) {
            s.push_back(chars[char_dist(gen)]);
        }
        v2.insert(std::move(s));
    }
    t2 = std::chrono::steady_clock::now();
    d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

    N = 500'000;
    v2.clear();

    t1 = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < N; i++) {
        std::string s;
        std::size_t len = len_dist(gen);
        for (std::size_t j = 0; j < len; j++) {
            s.push_back(chars[char_dist(gen)]);
        }
        v2.insert(std::move(s));
    }
    t2 = std::chrono::steady_clock::now();
    d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

}