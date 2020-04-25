#include <iostream>

int main() {
    size_t len = 0;
    constexpr size_t MAX_BUF = 256;
    char c;
    char* buffer = new char[MAX_BUF];
    while (std::cin >> c && len < MAX_BUF) {
        buffer[len++] = c;
        if (c == '!') break;
    }
    std::cout << buffer;
}