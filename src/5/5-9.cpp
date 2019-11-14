#include <iostream>
#include <numeric>
#include <vector>

int main() {
    int count = 0;
    std::cout << "Please enter the number of values you want to sum: ";
    while (true) {
        std::cin >> count;
        if (!std::cin) {
            std::cout << "Wrong input, try again.";
        } else if (count < 0) {
            std::cout << "Number is negative, try again.";
        } else {
            break;
        }
    }
    std::vector<int> ints;
    std::cout << "Please enter some integers (press '|' to stop) ";
    std::string input;
    while (std::cin >> input) {
        if (input == "|") {
            break;
        } else {
            try {
                int num = std::stoi(input);
                ints.push_back(num);
            } catch (std::invalid_argument& e) {
                std::cout << "Wrong number!\n";
            }
        }
    }
    if (ints.size() < count) {
        std::cout << "Sorry, you asked a sum of more numbers in the vector\n";
        return EXIT_FAILURE;
    }
    int sum = std::accumulate(ints.begin(), ints.begin() + count, 0);
    std::cout << "The sum of first " << count << " numbers are: " << sum;


}