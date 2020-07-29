#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <regex>

int main() {

    std::ifstream ifs {"text.txt"};
    if (!ifs) {
        throw std::runtime_error("no file");
    }

    std::vector<std::string> vs;
    std::string s;
    while (ifs >> s) {
        vs.push_back(s);
    }

    std::regex date_regex(R"(^(\d{1,2})/(\d{1,2})/(\d{4}))");
    std::smatch date_match;
    for (const auto& line : vs) {
        if (std::regex_search(line, date_match, date_regex)) {
            std::cout << date_match.prefix();
            for (size_t i = date_match.size() - 1; i < date_match.size(); i--) {
                std::cout << date_match[i];
                if (i != 0) {
                    std::cout << '-';
                }
            }
            std::cout << date_match.suffix() << "\n";
        }
    }

}