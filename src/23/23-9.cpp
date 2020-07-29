#include <fstream>
#include <stdexcept>
#include <string>
#include <regex>

int main()
{
    std::ifstream in {"table.txt"};        // input file
    if (!in) throw std::runtime_error("no input file\n");

    std::string line;                                // input buffer
    int lineno = 0;

    std::regex header {R"(^[\w ]+(           [\w ]+)*$)"};                      // header line
    std::regex row {R"(^[\w ]+(              \d+)(  \d+)(   \d+)$)"};        // data line

    if (getline(in,line)) {                  // check header line
        std::smatch matches;
        if (!std::regex_match(line, matches, header))
            throw std::runtime_error("no header");
    }
    while (getline(in,line)) {         // check data line
        ++lineno;
        std::smatch matches;
        if (!std::regex_match(line, matches, row))
            throw std::runtime_error("bad line" + std::to_string(lineno));
    }
}