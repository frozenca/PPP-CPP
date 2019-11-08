#include <iostream>
#include <string>

int main() {
    int n = 0;
    int left = 0, right = 100;
    std::string s;
    while (left < right) {
        if (right - left == 1) break;
        int mid = left + (right - left) / 2;
        std::cout << "Is the number you are thinking of less than " << mid << "? (Yes or No)\n";
        while (std::cin >> s) {
            if (s == "Yes") {
                right = mid;
                break;
            } else if (s == "No") {
                left = mid;
                break;
            } else {
                std::cout << "That's wrong, try again.\n";
            }
        }
    }
    std::cout << "The number is : " << left << '\n';
}