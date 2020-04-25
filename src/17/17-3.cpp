#include <iostream>

constexpr int diff = 'a' - 'A';

void to_lower(char* s) {
    while (*s) {
        if (*s >= 'A' && *s <= 'Z') {
            *s += diff;
        }
        s++;
    }
}

int main() {
    char s[] = "Hello, World!";
    to_lower(s);
    std::cout << s;
}