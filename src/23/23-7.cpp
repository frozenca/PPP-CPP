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

    std::regex card_regex(R"(^(?:4[0-9]{12}(?:[0-9]{3})?|[25][1-7][0-9]{14}|6(?:011|5[0-9][0-9])[0-9]{12}|3[47][0-9]{13}|3(?:0[0-5]|[68][0-9])[0-9]{11}|(?:2131|1800|35\d{3})\d{11}))");
    std::smatch card_match;
    for (const auto& line : vs) {
        if (std::regex_search(line, card_match, card_regex)) {
            std::cout << "matches for '" << line << "'\n";
            std::cout << "Prefix: '" << card_match.prefix() << "'\n";
            for (size_t i = 0; i < card_match.size(); ++i)
                std::cout << i << ": " << card_match[i] << '\n';
            std::cout << "Suffix: '" << card_match.suffix() << "\'\n\n";
        }
    }

}