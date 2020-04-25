#include <iostream>

char* strdup(const char* s) {
    size_t len = 0;
    const char* p = s;
    while (*p++) {
        len++;
    }
    char* result = new char[len + 1];
    for (size_t i = 0; i <= len; i++) {
        result[i] = s[i];
    }
    return result;
}

int main() {
    char s[] = "Hello, World!";
    char* p = strdup(s);
    std::cout << p;
}