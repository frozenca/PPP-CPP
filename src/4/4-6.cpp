#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> nums {"zero", "one", "two", "three", "four", "five", "six",
                                   "seven", "eight", "nine"};

    int n = 0;
    std::cin >> n;
    if (n < 10 && n >= 0) {
        std::cout << nums[n] << '\n';
    } else {
        std::cout << "Sorry, I don't know the number\n";
    }
    std::string s;
    std::cin >> s;
    auto it = std::find(nums.begin(), nums.end(), s);
    if (it != nums.end()) {
        std::cout << std::distance(nums.begin(), it);
    } else {
        std::cout << "Sorry, nums doesn't have the string you input\n";
    }

}