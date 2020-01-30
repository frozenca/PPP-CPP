#include <iostream>
#include <iomanip>

int main() {
    std::string str;
    while (std::cin >> str) {
        char* end;
        unsigned long long x = std::strtoll(str.c_str(), &end, 0);
        std::string base = "decimal";
        if (str.substr(0, 2) == "0x") {
            base = "hexadecimal";
        } else if (str.substr(0, 1) == "0") {
            base = "octal";
        }
        std::cout << std::setw(6) << str << std::setw(12) << base << std::setw(12)
        << " converts to " << std::setw(6) << x << " decimal\n";
    }

}