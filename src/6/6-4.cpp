#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>

struct Name_value {
    std::string name;
    int score;
    Name_value (std::string _name, int _score) : name{std::move(_name)}, score{_score} {}
};

int main() {
    std::string n;
    int v = 0;
    std::vector<Name_value> name_values;
    while (true){
        std::cin >> n >> v;
        if (n == "NoName" && v == 0) {
            break;
        }
        name_values.emplace_back(n, v);
    }
    std::vector<Name_value> sorted(name_values.begin(), name_values.end());
    std::sort(sorted.begin(), sorted.end());
    bool dup = false;
    for (int i = 1; i < sorted.size(); i++) {
        if (sorted[i].name == sorted[i - 1].name) {
            dup = true;
            break;
        }
    }
    if (dup) {
        std::cerr << "Sorry, there's a duplicated name\n";
        return EXIT_FAILURE;
    }
    for (const auto& [name, score] : name_values) {
        std::cout << name << ' ' << score << '\n';
    }

}