#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <utility>
#include <string_view>

std::pair<int, int> guess(const std::vector<char> &nums, const std::unordered_set<char> &numset, std::string_view sv) {
    int bulls = 0, cows = 0;
    for (size_t i = 0; i < sv.size(); i++) {
        char c = sv[i];
        if (nums[i] == c) {
            bulls++;
        } else if (numset.find(c) != numset.end()) {
            cows++;
        }
    }
    return {bulls, cows};
}

int main() {
    std::vector<char> chars{'t', 'r', 'e', 'e'};
    std::unordered_set<char> charset;
    for (auto c : chars) {
        charset.insert(c);
    }

    std::string n;
    std::cout << "Input your guess:\n";

    while (std::cin >> n) {
        try {
            auto [bulls, cows] = guess(chars, charset, n);
            std::cout << bulls << " bulls, " << cows << " cows for " << n << "\n";
            if (bulls == 4) {
                std::cout << "Congrats!\n";
                break;
            }
        } catch (std::invalid_argument &e) {
            std::cerr << e.what() << "\n";
        }
    }
}