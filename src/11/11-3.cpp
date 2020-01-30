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
        switch (std::tolower(ch)) {
            case 'a':
                [[fallthrough]];
            case 'e':
                [[fallthrough]];
            case 'i':
                [[fallthrough]];
            case 'o':
                [[fallthrough]];
            case 'u':
                break;
            default:
                ofs << static_cast<char>(ch);
        }

    }

}