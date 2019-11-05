#include <iostream>
#include <string>

int main() {
    std::cout << "Write a number of pennies: \n";
    int pennies = 0;
    std::cin >> pennies;
    std::cout << "Write a number of nickels: \n";
    int nickels = 0;
    std::cin >> nickels;
    std::cout << "Write a number of dimes: \n";
    int dimes = 0;
    std::cin >> dimes;
    std::cout << "Write a number of quarters: \n";
    int quarters = 0;
    std::cin >> quarters;
    std::cout << "Write a number of half dollars: \n";
    int half_dollars = 0;
    std::cin >> half_dollars;
    double money = (1 * pennies + 5 * nickels + 10 * dimes + 25 * quarters + 50 * half_dollars) / 100.0;
    std::cout << "You have " << pennies << ((pennies == 1) ? " penny.\n" : " pennies.\n")
            << "You have " << nickels << ((nickels == 1) ? " nickel.\n" : " nickels.\n")
            << "You have " << dimes << ((dimes == 1) ? " dime.\n" : " dimes.\n")
            << "You have " << quarters << ((quarters == 1) ? " quarter.\n" : " quarters.\n")
            << "You have " << half_dollars << ((half_dollars == 1) ? " half dollar.\n" : " half dollars.\n")
            << "The value of all of your coins is $" << money << ".\n";
}