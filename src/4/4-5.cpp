#include <iostream>
#include <string>

int main() {
    double num1 = 0.0, num2 = 0.0;
    char op;
    std::cin >> num1 >> num2 >> op;
    switch(op) {
        case '+':
            std::cout << "The sum of " << num1 << " and " << num2 << " is " << (num1 + num2);
            break;
        case '-':
            std::cout << "The difference of " << num1 << " and " << num2 << " is " << (num1 - num2);
            break;
        case '*':
            std::cout << "The product of " << num1 << " and " << num2 << " is " << (num1 * num2);
            break;
        case '/':
            std::cout << "The quotient of " << num1 << " and " << num2 << " is " << (num1 / num2);
            break;
        default:
            std::cout << "Sorry, I don't know operator " << op;
    }


}