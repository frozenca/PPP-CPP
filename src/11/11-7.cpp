#include <iostream>
#include <algorithm>
#include <string>

int main() {
    std::string str;
    while (std::cin >> str) {
        for (char& c : str) {
            c = static_cast<char>(std::tolower(c));
        }
        std::string::size_type n = str.find("don't");
        if (n != std::string::npos) {
            str.replace(n, 5, "do not");
        }
        n = str.find("didn't");
        if (n != std::string::npos) {
            str.replace(n, 6, "did not");
        }
        n = str.find("can't");
        if (n != std::string::npos) {
            str.replace(n, 5, "cannot");
        }
        n = str.find("couldn't");
        if (n != std::string::npos) {
            str.replace(n, 8, "could not");
        }
        n = str.find("won't");
        if (n != std::string::npos) {
            str.replace(n, 5, "will not");
        }
        n = str.find("wouldn't");
        if (n != std::string::npos) {
            str.replace(n, 8, "would not");
        }
        n = str.find("i'm");
        if (n != std::string::npos) {
            str.replace(n, 3, "i am");
        }
        n = str.find("you're");
        if (n != std::string::npos) {
            str.replace(n, 6, "you are");
        }
        n = str.find("he's");
        if (n != std::string::npos) {
            str.replace(n, 4, "he is");
        }
        for (size_t i = 1; i < str.size() - 1; i++) {
            if (std::ispunct(str[i]) && std::isalpha(str[i - 1]) && std::isalpha(str[i + 1])) {
                str[i] = '#';
            }
        }
        std::erase_if(str, [](char x) {return std::ispunct(x);});
        str.erase(std::remove(str.begin(), str.end(), '#'));

    }
}