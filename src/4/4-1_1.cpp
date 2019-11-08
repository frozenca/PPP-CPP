#include <iostream>

int main() {
    constexpr double yen_per_dollar = 0.0092;
    constexpr double kroner_per_dollar = 0.11;
    constexpr double pound_per_dollar = 1.28;
    double money = 1;
    char unit = 0;
    std::cout << "Please enter amount of money followed by a unit (y or k or p):\n";
    std::cin >> money >> unit;
    if (unit == 'y') {
        std::cout << money << " yens == " << money * yen_per_dollar << " dollars\n";
    } else if (unit == 'k') {
        std::cout << money << " kroners == " << money * kroner_per_dollar << " dollars\n";
    } else if (unit == 'p') {
        std::cout << money << " pounds == " << money * pound_per_dollar << " dollars\n";
    } else {
        std::cout << "Sorry, I don't know a unit called '" << unit << "'\n";
    }
}