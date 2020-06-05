#include <cassert>
#include <fstream>

struct FileHandler {
    std::fstream fs;
    FileHandler (const std::string& filename) {
        fs.open(filename, fs.binary | fs.in | fs.out);
    }

    ~FileHandler() = default;
};

int main() {
}