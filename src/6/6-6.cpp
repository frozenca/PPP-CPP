#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> conjunctions {"and", "or", "but"};
    std::vector<std::string> nouns {"birds", "fish", "C++"};
    std::vector<std::string> verbs {"rules", "fly", "swim"};

    std::string str;
    bool noun = false;
    bool conj = false;
    bool verb = false;
    bool OK = false;
    while (std::cin >> str) {
        if (str == ".") {
            OK = verb;
            break;
        } else if (std::find(nouns.begin(), nouns.end(), str) != nouns.end()) { // noun
            if (noun) { // noun-noun
                break;
            } else { // noun or conj-noun
                noun = true;
                conj = false;
            }
        } else if (std::find(verbs.begin(), verbs.end(), str) != verbs.end()) { // verb
            if (noun) { // noun-verb
                verb = true;
                noun = false;
            } else { //
                break;
            }
        } else if (std::find(conjunctions.begin(), conjunctions.end(), str) != conjunctions.end()) { // conj
            if (verb) { // (noun-verb)-conj
                conj = true;
                verb = false;
            } else {
                break;
            }
        } else if (str == "the") {
            continue;
        } else {
            OK = false;
            break;
        }
    }
    std::cout << (OK ? "OK" : "not OK");

}