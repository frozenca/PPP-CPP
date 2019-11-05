#include <iostream>
#include <string>

int main() {
    std::cout << "Write an operator followed by two operands: \n";
    std::string op;
    double val1 = 0.0, val2 = 0.0;
    std::cin >> op >> val1 >> val2;
    if (op == "+" || op == "plus") {
        std::cout << val1 << " + " << val2 << " = " << (val1 + val2);
    } else if (op == "-" || op == "minus") {
        std::cout << val1 << " - " << val2 << " = " << (val1 - val2);
    } else if (op == "*" || op == "mul") {
        std::cout << val1 << " * " << val2 << " = " << (val1 * val2);
    } else if (op == "/" || op == "div") {
        std::cout << val1 << " / " << val2 << " = " << (val1 / val2);
    } else {
        std::cout << "Invalid operator";
    }

}