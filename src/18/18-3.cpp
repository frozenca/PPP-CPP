#include <cstddef>
#include <iostream>

int strcmp(const char* s1, const char* s2) {
    unsigned char u1, u2;
    while (true) {
        u1 = *s1++;
        u2 = *s2++;
        if (u1 != u2)
            return u1 - u2;
        if (!u1 || !u2)
            return 0;
    }
    return 0;
}

int main() {

}