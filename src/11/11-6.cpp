#include <iostream>

int main() {
    std::string str;
    for (char ch; std::cin.get(ch); std::cout << str, str = "") {
        if (std::ispunct(ch)) {
            str += ' ';
        } else {
            str += ch;
        }
    }
}