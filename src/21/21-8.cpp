#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::map<std::string, size_t> words;         // keep (word,frequency) pairs

    for (std::string s; std::cin >> s;) {
        ++words[s];
    }

    std::vector<std::pair<std::string, size_t>> values(words.begin(), words.end());
    auto cmp = [](const auto& l, const auto& r) {
        return l.second < r.second;
    };
    std::sort(values.begin(), values.end(), cmp);

    for (const auto& [key, freq] : values) {
        std::cout << freq << " : " << key << '\n';
    }

}