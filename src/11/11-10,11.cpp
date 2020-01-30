#include <sstream>
#include <vector>
#include <string>

std::vector<std::string> split(const std::string& data,
        const std::string& w = "") {
    std::vector<std::string> result;
    std::string token;
    for (char c : data) {
        if (std::isspace(c) || w.find(c) != std::string::npos) {
            token += c;
        } else {
            result.push_back(token);
            token = "";
        }
    }
    return result;
}

int main() {
    auto ans1 = split("Do you like fish?");
    auto ans2 = split("Do you like fish?", "i");

}