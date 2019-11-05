#include <cmath>
#include <iostream>

// simple program to exercise operators
int main() {
    std::cout << "Please enter an integer value: ";
    int n;
    std::cin >> n;
    double d = n;
    std::cout << "n == " << n
              << "\nn+1 == " << n + 1
              << "\nthree times n == " << 3 * n
              << "\ntwice n == " << n + n
              << "\nn squared == " << n * n
              << "\nhalf of n == " << n / 2
              << "\nsquare root of n == " << std::sqrt(d)
              << '\n';  // another name for newline (“end of line”) in output
}