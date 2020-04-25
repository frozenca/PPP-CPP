#include <iostream>
#include <cstring>

char* findx(const char* s, const char* x) {
    return const_cast<char *>(std::strstr(s, x));
}

int main() {
    char s[] = "Hello, World!";
    char p[] = "ell";
    char* q = findx(s, p);
    std::cout << q;
}