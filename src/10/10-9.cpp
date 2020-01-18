#include <fstream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>

int main() {
    std::ifstream ifs1 {"input1.txt"};
    if (!ifs1) {
        throw std::runtime_error("input1 open failed");
    }
    std::ifstream ifs2 {"input2.txt"};
    if (!ifs2) {
        throw std::runtime_error("input2 open failed");
    }
    std::ofstream ofs {"output.txt"};
    if (!ofs) {
        throw std::runtime_error("output open failed");
    }
    std::vector<std::string> vs;
    std::string temp;
    while (ifs1 >> temp) {
        vs.push_back(temp);
    }
    while (ifs2 >> temp) {
        vs.push_back(temp);
    }
    std::sort(vs.begin(), vs.end());
    for (const auto& s : vs) {
        ofs << s << ' ';
    }
}