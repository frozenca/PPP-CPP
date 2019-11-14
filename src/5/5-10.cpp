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
    std::vector<double> nums;
    std::cout << "Please enter some numbers (press '|' to stop) ";
    std::string input;
    while (std::cin >> input) {
        if (input == "|") {
            break;
        } else {
            try {
                double num = std::stod(input);
                nums.push_back(num);
            } catch (std::invalid_argument& e) {
                std::cout << "Wrong number!\n";
            }
        }
    }
    if (nums.size() < count) {
        std::cout << "Sorry, you asked a sum of more numbers in the vector\n";
        return EXIT_FAILURE;
    }
    std::vector<double> diffs;
    for (int i = 0; i < count - 1; i++) {
        diffs.push_back(nums[i + 1] - nums[i]);
    }
    std::cout << "The difference of adjacent elements are:\n";
    for (const auto diff : diffs) {
        std::cout << diff << ' ';
    }


}