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
    for (int i = 1; i <= m; i++) {
        if (is_prime(i)) {
            primes.push_back(i);
        }
    }
    for (auto p : primes) {
        std::cout << p << ' ';
    }


}