#include <iostream>
#include <string>

int main() {
    std::string previous = " ";       // previous word; initialized to “not a word”
    std::string current;                             // current word
    while (std::cin >> current) {                // read a stream of words
        if (previous == current)    // check if the word is the same as last
            std::cout << "repeated word: " << current << '\n';
        previous = current;
    }
}