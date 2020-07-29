#include <fstream>
#include <stdexcept>
#include <string>
#include <regex>
#include <iostream>
#include <charconv>
#include <tuple>
#include <map>

int main() {
    std::ifstream in{"table.txt"};        // input file
    if (!in) throw std::runtime_error("no input file\n");

    std::string line;                                // input buffer
    int lineno = 0;

    std::regex header{R"(^[\w ]+(           [\w ]+)*$)"};                      // header line
    std::regex row{R"(^[\w ]+(              \d+)(  \d+)(   \d+)$)"};        // data line

    if (getline(in, line)) {                  // check header line
        std::smatch matches;
        if (!std::regex_match(line, matches, header))
            throw std::runtime_error("no header");
    }
    // column totals:
    int boys = 0;
    int girls = 0;

    std::map<int, std::tuple<int, int, int>> m;
    while (getline(in, line)) {
        ++lineno;
        std::smatch matches;
        if (!std::regex_match(line, matches, row))
            std::cerr << "bad line: " << lineno << '\n';

        if (in.eof()) std::cout << "at eof\n";

        // check row:
        int curr_year = matches[1].str()[0];
        int curr_boy = std::from_chars(matches[2].str().c_str());
        int curr_girl = std::from_chars(matches[3].str().c_str());
        int curr_total = std::from_chars(matches[4].str().c_str());
        if (curr_boy + curr_girl != curr_total) throw std::runtime_error("bad row sum \n");

        auto&& [year_boy, year_girl, year_total] = m[curr_year];
        year_boy += curr_boy;
        year_girl += curr_girl;
        year_total += curr_total;

        if (matches[1] == "Alle klasser") {           // last line
            if (curr_boy != boys) throw std::runtime_error("boys don't add up\n");
            if (curr_girl != girls) throw std::runtime_error("girls don't add up\n");
            if (!(in >> std::ws).eof()) throw std::runtime_error("characters after total line");

            for (const auto& [yr, tp] : m) {
                auto [b, g, t] = tp;
                std::cout << yr << ' ' << b << ' ' << g << ' ' << t << '\n';
            }
            return 0;
        }

        // update totals:
        boys += curr_boy;
        girls += curr_girl;
    }


    throw std::runtime_error("didn't find total line");
}