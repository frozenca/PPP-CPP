#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>

int main() {
    std::vector<std::string> words;
    for (std::string word; std::cin >> word; ) {
        words.push_back(word);
    }
    for (auto& word : words) {
        std::replace(word.begin(), word.end(), ';', ' ');
        std::transform(word.begin(), word.end(), word.begin(),
                [](auto c) {return std::tolower(c);});
        std::string::size_type n {0};
        while (n != std::string::npos) {
            n = word.find("n't");
            word.replace(n, 3, "not");
        }
        if (!word.empty() && word.back() == 's') {
            word.pop_back();
        }
    }
    int ships = std::count(words.begin(), words.end(), "ship");
    std::map<std::string, size_t> wordByFreq;
    for (const auto& word : words) {
        ++wordByFreq[word];
    }
    std::vector<std::pair<std::string, size_t>> wordSortedByFreq (wordByFreq.begin(), wordByFreq.end());
    auto comp = [](auto& l, auto& r) {return l.second < r.second;};
    std::sort(wordSortedByFreq.begin(), wordSortedByFreq.end(), comp);

    auto length_compare = [](auto& l, auto& r) {return l.size() < r.size();};
    std::string longest = *std::max_element(words.begin(), words.end(), length_compare);
    std::string shortest = *std::min_element(words.begin(), words.end(), length_compare);

    std::vector<std::string> startsWithS;
    for (const auto& word : words) {
        if (word.starts_with('s')) {
            startsWithS.push_back(word);
        }
    }

    std::vector<std::string> fourLength;
    for (const auto& word : words) {
        if (word.size() == 4) {
            fourLength.push_back(word);
        }
    }

}