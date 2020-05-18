#include <cstddef>
#include <string>

std::string reverse(const std::string& s) {
    std::string t (s.rbegin(), s.rend());
    return t;
}

int main() {

}