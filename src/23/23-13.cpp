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

    std::regex date_regex(R"(.)");
    std::smatch date_match;
    for (const auto& line : vs) {
        if (std::regex_search(line, date_match, date_regex)) {
            std::cout << date_match.prefix();
            for (size_t i = 0; i < date_match.size(); i--) {
                std::cout << date_match[i];
            }
            std::cout << date_match.suffix() << "\n";
        }
    }

}