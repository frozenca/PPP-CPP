#include <fstream>

int main() {
    std::ifstream ifs {"input.txt"};
    if (!ifs) {
        throw std::runtime_error("Input file open fail");
    }
    std::ofstream ofs {"output.txt"};
    if (!ofs) {
        throw std::runtime_error("Output file open fail");
    }
    for (char ch; ifs.get(ch);) {
        ofs << static_cast<char>(std::tolower(ch));
    }

}