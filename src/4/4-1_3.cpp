#include <iostream>

int main() {
    char ch = 'a';
    while (ch <= 'z') {
        std::cout << ch << "   " << int(ch) << '\n';
        ch++;
    }
}