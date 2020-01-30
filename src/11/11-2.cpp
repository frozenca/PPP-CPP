#include <fstream>
#include <string>

int main() {
    std::ifstream ifs {"input.txt"};
    if (!ifs) {
        throw std::runtime_error("Input file open fail");
    }
    std::ofstream ofs {"output.txt"};
    if (!ofs) {
        throw std::runtime_error("Output file open fail");
    }
    std::string line;
    size_t count = 0;
    std::string word {"fish"};
    while(std::getline(ifs, line)) {
        count++;
        if (line.find(word) != std::string::npos) {
            ofs << count << ' ' << line << '\n';
        }
    }

}