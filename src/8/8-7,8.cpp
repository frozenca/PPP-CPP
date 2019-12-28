#include <iostream>
#include <vector>
#include <string>
#include <algorithm>


int main() {
    std::vector<std::pair<std::string, double>> name_age;
    std::cout << "Input names: (stop with 'Q') \n";
    std::string str;
    while (std::cin >> str) {
        if (str == "Q") break;
        name_age.emplace_back(str, 0.0);
    }
    std::cout << "Input ages: \n";
    double d = 0.0;
    size_t i = 0;
    while (std::cin >> d) {
        if (i >= name_age.size()) {
            break;
        }
        name_age[i++].second = d;
    }
    std::sort(name_age.begin(), name_age.end());

    for (const auto& [name, age] : name_age) {
        std::cout << name << ' ' << age << ' ';
    }

}