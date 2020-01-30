#include <iostream>
#include <iomanip>

int main() {

    size_t space = 0, alpha = 0,
    digit = 0, xdigit = 0, upper = 0, lower = 0,
    alnum = 0, cntrl = 0, punct = 0, print = 0, graph = 0;

    for (char ch; std::cin.get(ch);) {
        if (std::isspace(ch)) {
            space++;
        }
        if (std::isalpha(ch)) {
            alpha++;
        }
        if (std::isdigit(ch)) {
            digit++;
        }
        if (std::isxdigit(ch)) {
            xdigit++;
        }
        if (std::isupper(ch)) {
            upper++;
        }
        if (std::islower(ch)) {
            lower++;
        }
        if (std::isalnum(ch)) {
            alnum++;
        }
        if (std::iscntrl(ch)) {
            cntrl++;
        }
        if (std::ispunct(ch)) {
            punct++;
        }
        if (std::isprint(ch)) {
            print++;
        }
        if (std::isgraph(ch)) {
            graph++;
        }
    }
    std::cout << "We have "
            << space << " spaces, "
            << alpha << " alphas, "
            << digit << " digits, "
            << xdigit << " xdigits, "
            << upper << " uppers, "
            << lower << " lowers, "
            << alnum << " alnums, "
            << alnum << " alnums, "
            << cntrl << " cntrls, "
            << punct << " puncts, "
            << print << " prints, "
            << graph << " graphs. ";

}