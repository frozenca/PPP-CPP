#include <iostream>
#include <vector>

bool is_prime(int n) {
    if (n == 1) return false;
    if (n == 2) return true;
    for (int k = 2; k * k <= n; k++) {
        if (n % k == 0) return false;
    }
    return true;
}

int main() {
    int m = 0;
    std::cin >> m;
    std::vector<int> primes;
    int n = 1, count = 0;
    while (count < m) {
        if (is_prime(n)) {
            primes.push_back(n);
            count++;
        }
        n++;
    }
    for (auto p : primes) {
        std::cout << p << ' ';
    }


}