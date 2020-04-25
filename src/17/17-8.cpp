#include <iostream>

int main() {
    size_t len = 0;
    char c;
    std::string s;
    while (std::cin >> c) {
        s.push_back(c);
        if (c == '!') break;
    }
    std::cout << s;
}