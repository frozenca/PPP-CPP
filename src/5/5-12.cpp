#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <utility>

std::pair<int, int> guess(const std::vector<int>& nums, const std::unordered_set<int>& numset, int n) {
    int digit = 0;
    int bulls = 0, cows = 0;
    while (n) {
        int k = n % 10;
        if (nums[nums.size() - 1 - digit] == k) {
            bulls++;
        } else if (numset.find(k) != numset.end()) {
            cows++;
        }
        digit++;
        n /= 10;
    }
    return {bulls, cows};
}

int main() {
    std::vector<int> nums {1, 3, 5, 9};
    std::unordered_set<int> numset;
    for (auto n : nums) {
        numset.insert(n);
    }

    std::string n;
    std::cout << "Input your guess:\n";

    while (std::cin >> n) {
        try {
            int a = std::stoi(n);
            if (a > 9999) {
                throw std::invalid_argument("Out of range");
            } else if (a < 0) {
                throw std::invalid_argument("Negative number");
            }
            auto [bulls, cows] = guess(nums, numset, a);
            std::cout << bulls << " bulls, " << cows << " cows for " << a << "\n";
            if (bulls == 4) {
                std::cout << "Congrats!\n";
                break;
            }
        } catch (std::invalid_argument& e) {
            std::cerr << e.what() << "\n";
        } catch (std::exception& e) {
            std::cerr << "This is not a number, try again.\n";
        }
    }





}