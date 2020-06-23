#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <list>
#include <random>
#include <ratio>
#include <set>
#include <vector>

int main() {
    std::mt19937 gen(std::random_device{}());
    constexpr size_t NUM_TRIALS = 10'000;

    for (size_t N = 128; N <= (1u << 11u); N <<= 1u) {
        std::chrono::microseconds Dt1(0), Dt2(0), Dt3(0);
        for (size_t t = 0; t < NUM_TRIALS; t++) {
            std::vector<int> v;
            std::list<int> l;
            std::set<int> s;
            std::uniform_int_distribution<> dist(0, N);

            auto t1 = std::chrono::steady_clock::now();
            // insert into vector
            for (size_t i = 0; i < N; i++) {
                int n = dist(gen);
                auto pos = std::lower_bound(v.begin(), v.end(), n);
                v.insert(pos, n);
            }
            auto t2 = std::chrono::steady_clock::now();
            auto dt1 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
            Dt1 += dt1;
            assert(std::is_sorted(v.begin(), v.end()));

            auto t3 = std::chrono::steady_clock::now();
            // insert into list
            for (size_t i = 0; i < N; i++) {
                int n = dist(gen);
                auto pos = std::lower_bound(l.begin(), l.end(), n);
                l.insert(pos, n);
            }
            auto t4 = std::chrono::steady_clock::now();
            auto dt2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);
            Dt2 += dt2;
            assert(std::is_sorted(l.begin(), l.end()));

            auto t5 = std::chrono::steady_clock::now();
            // insert into set
            for (size_t i = 0; i < N; i++) {
                int n = dist(gen);
                s.insert(n);
            }
            auto t6 = std::chrono::steady_clock::now();
            auto dt3 = std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5);
            Dt3 += dt3;
        }
        std::cout << "Inserting " << N << " elements in a vector, keeping the vector sorted: "
                  << Dt1.count() / NUM_TRIALS << "us\n";
        std::cout << "Inserting " << N << " elements in a list, keeping the list sorted: " << Dt2.count() / NUM_TRIALS
                  << "us\n";
        std::cout << "Inserting " << N << " elements in a set: " << Dt3.count() / NUM_TRIALS << "us\n";
    }
}