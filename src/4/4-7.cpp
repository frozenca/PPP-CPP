#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> nums {"zero", "one", "two", "three", "four", "five", "six",
                                   "seven", "eight", "nine"};

    std::string num1, num2;
    char op;
    std::cin >> num1 >> num2 >> op;

    int n1 = 0, n2 = 0;
    auto it1 = std::find(nums.begin(), nums.end(), num1);
    if (it1 != nums.end()) {
        n1 = std::distance(nums.begin(), it1);
    } else if (num1[0] >= '0' && num1[0] <= '9') {
        n1 = num1[0] - '0';
    } else {
        std::cerr << "Sorry, I don't know the number " << n1;
        return EXIT_FAILURE;
    }
    auto it2 = std::find(nums.begin(), nums.end(), num2);
    if (it2 != nums.end()) {
        n2 = std::distance(nums.begin(), it2);
    } else if (num2[0] >= '0' && num2[0] <= '9') {
        n2 = num2[0] - '0';
    } else {
        std::cerr << "Sorry, I don't know the number " << n2;
        return EXIT_FAILURE;
    }

    switch(op) {
        case '+':
            std::cout << "The sum of " << num1 << " and " << num2 << " is " << (n1 + n2);
            break;
        case '-':
            std::cout << "The difference of " << num1 << " and " << num2 << " is " << (n1 - n2);
            break;
        case '*':
            std::cout << "The product of " << num1 << " and " << num2 << " is " << (n1 * n2);
            break;
        case '/':
            std::cout << "The quotient of " << num1 << " and " << num2 << " is " << (n1 / n2);
            break;
        default:
            std::cout << "Sorry, I don't know operator " << op;
    }

}