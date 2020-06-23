#include <vector>
#include <iostream>

template <typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& vec) {
    size_t sz = 0;
    if (is >> sz) {
        size_t i = 0;
        for (T elem; is >> elem && i < sz; i++) {
            vec.push_back(elem);
        }
        if (!is.eof()) {
            if (is.bad()) {
                throw std::runtime_error("Stream corrupted");
            }
        }
    }
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    for (const auto& elem : vec) {
        os << elem << ' ';
    }
    os << '\n';
}

int main() {

}