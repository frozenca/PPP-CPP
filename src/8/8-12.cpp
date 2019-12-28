#include <iostream>
#include <vector>
#include <string>

void print_until_ss(const std::vector<std::string>& v, const std::string& quit) {
    bool to_quit = false;
    for (const auto& s : v) {
        if (s == quit) {
            if (!to_quit) {
                to_quit = true;
            } else {
                break;
            }
        }
        std::cout << s << '\n';
    }
}

int main() {
    std::vector<std::string> v {"1", "2", "3", "1", "4", "5"};
    print_until_ss(v, "1");
}