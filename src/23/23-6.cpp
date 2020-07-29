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

    std::regex date_regex(R"(^\d{1,2}/\d{1,2}/\d{4})");
    std::smatch date_match;
    for (const auto& line : vs) {
        if (std::regex_search(line, date_match, date_regex)) {
            std::cout << "matches for '" << line << "'\n";
            std::cout << "Prefix: '" << date_match.prefix() << "'\n";
            for (size_t i = 0; i < date_match.size(); ++i)
                std::cout << i << ": " << date_match[i] << '\n';
            std::cout << "Suffix: '" << date_match.suffix() << "\'\n\n";
        }
    }

}