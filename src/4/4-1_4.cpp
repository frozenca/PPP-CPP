#include <iostream>

int main() {
    for (char ch = 'a'; ch <= 'z'; ch++) {
        std::cout << ch << "   " << int(ch) << '\n';
    }
}