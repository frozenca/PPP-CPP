#include <cstddef>
#include <string>

std::string cat(std::string_view s1, std::string_view s2, std::string_view sep = ".") {
    std::string s;
    s += s1;
    s += sep;
    s += s2;
    return s;
}

int main() {

}