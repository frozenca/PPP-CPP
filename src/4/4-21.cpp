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
    std::vector<int> sorted(names.begin(), names.end());
    std::sort(sorted.begin(), sorted.end());
    bool dup = false;
    for (int i = 1; i < sorted.size(); i++) {
        if (sorted[i] == sorted[i - 1]) {
            dup = true;
            break;
        }
    }
    if (dup) {
        std::cerr << "Sorry, there's a duplicated score\n";
        return EXIT_FAILURE;
    }
    int score;
    std::cin >> score;
    auto it = std::find(scores.begin(), scores.end(), score);
    if (it != scores.end()) {
        std::cout << names[std::distance(scores.begin(), it)];
    } else {
        std::cout << "name not found\n";
    }

}