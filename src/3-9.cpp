#include <iostream>
#include <string>

int main() {
    std::cout << "Write a digit in word:\n";
    std::string a;
    std::cin >> a;
    if (a == "zero") {
        std::cout << "0\n";
    } else if (a == "one") {
        std::cout << "1\n";
    } else if (a == "two") {
        std::cout << "2\n";
    } else if (a == "three") {
        std::cout << "3\n";
    } else if (a == "four") {
        std::cout << "4\n";
    } else {
        std::cout << "Not a number I know\n";
    }

}