#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::string n;
    int v = 0;
    std::vector<std::string> names;
    std::vector<int> scores;
    while (true){
        std::cin >> n >> v;
        if (n == "NoName" && v == 0) {
            break;
        }
        names.push_back(n);
        scores.push_back(v);
    }
    std::vector<std::string> sorted(names.begin(), names.end());
    std::sort(sorted.begin(), sorted.end());
    bool dup = false;
    for (int i = 1; i < sorted.size(); i++) {
        if (sorted[i] == sorted[i - 1]) {
            dup = true;
            break;
        }
    }
    if (dup) {
        std::cerr << "Sorry, there's a duplicated name\n";
        return EXIT_FAILURE;
    }
    for (int i = 0; i < names.size(); i++) {
        std::cout << names[i] << ' ' << scores[i] << '\n';
    }

}