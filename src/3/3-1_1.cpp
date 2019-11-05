#include <iostream>
#include <string>

int main() {
    std::cout << "Please enter your first name and age\n";
    std::string first_name = "???";      // string variable
    // ("???” means “don’t know the name”)
    double age = -1.0;         // integer variable (–1 means “don’t know the age”)
    std::cin >> first_name >> age;      // read a string followed by an integer
    std::cout << "Hello, " << first_name << " (age " << age * 12 << " months)\n";
}