#include <iostream>
#include <iomanip>

int main() {
    for (char ch; std::cin.get(ch);) {
        std::cout << ch << " is";
        if (std::isspace(ch)) {
            std::cout << " Space ";
        }
        if (std::isalpha(ch)) {
            std::cout << " Alpha ";
        }
        if (std::isdigit(ch)) {
            std::cout << " Digit ";
        }
        if (std::isxdigit(ch)) {
            std::cout << " Hexadecimal digit ";
        }
        if (std::isupper(ch)) {
            std::cout << " Uppercase ";
        }
        if (std::islower(ch)) {
            std::cout << " Lowercase ";
        }
        if (std::isalnum(ch)) {
            std::cout << " Letter/Decimal ";
        }
        if (std::iscntrl(ch)) {
            std::cout << " Control character ";
        }
        if (std::ispunct(ch)) {
            std::cout << " Punctuation character ";
        }
        if (std::isprint(ch)) {
            std::cout << " Printable ";
        }
        if (std::isgraph(ch)) {
            std::cout << " Graphic character ";
        }
        std::cout << '\n';
    }
}