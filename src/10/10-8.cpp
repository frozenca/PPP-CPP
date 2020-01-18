#include <fstream>
#include <array>

int main() {
    std::ifstream ifs1 {"input1.txt", std::ios_base::binary};
    if (!ifs1) {
        throw std::runtime_error("input1 open failed");
    }
    std::ifstream ifs2 {"input2.txt", std::ios_base::binary};
    if (!ifs2) {
        throw std::runtime_error("input2 open failed");
    }
    std::ofstream ofs {"output.txt", std::ios_base::binary};
    if (!ofs) {
        throw std::runtime_error("output open failed");
    }
    ofs << ifs1.rdbuf() << ifs2.rdbuf();
}