#include <cstddef>
#include <iostream>

char* strchr(const char* s, int c) {
    do {
        if (*s == c) {
            return const_cast<char*>(s);
        }
    } while (*s++);
    return nullptr;
}

size_t strlen(const char* s) {
    size_t i = 0;
    for (i = 0; ; ++i) {
        if (!s[i]) break;
    }
    return i;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    unsigned char u1, u2;
    while (n--) {
        u1 = *s1++;
        u2 = *s2++;
        if (u1 != u2)
            return u1 - u2;
        if (!u1)
            return 0;
    }
    return 0;
}

char* strstr(const char* s1, const char* s2) {
    const char* p = s1;
    const size_t len = strlen(s2);
    for (; (p = strchr(p, *s2)); p++) {
        if (!strncmp(p, s2, len)) {
            return const_cast<char*>(p);
        }
    }
    return nullptr;
}

int main() {

}