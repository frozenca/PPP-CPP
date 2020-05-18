#include <cstddef>
#include <iostream>

char* strdup(const char* str) {
    size_t len = 0;
    const char* s = str;
    while (*s) {
        len++;
        s++;
    }
    char* dst = new char[len + 1];
    char* d = dst;
    while ((*dst++ = *str++)) ;
    return d;
}

int main() {
    const char* str = "abcde";
    std::cout << strdup(str) << '\n';

}