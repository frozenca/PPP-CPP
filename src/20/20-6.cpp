#include <algorithm>
#include <vector>
#include <iostream>
#include <list>
#include <iterator>
#include <string>

using Line = std::vector<char>;

class Text_iterator {
    std::list<Line>::iterator ln;
    Line::iterator pos;
public:
    Text_iterator(std::list<Line>::iterator ll, Line::iterator pp) : ln {ll}, pos {pp} {}

    char& operator*() {return *pos;}
    Text_iterator& operator++();

    bool operator==(const Text_iterator& other) const {
        return ln == other.ln && pos == other.pos;
    }
    bool operator!=(const Text_iterator& other) const {
        return !(*this == other);
    }
};

struct Document {
    std::list<Line> line;
    Document() {line.emplace_back();}
    Text_iterator begin() {
        return Text_iterator(line.begin(), line.begin()->begin());
    }
    Text_iterator end() {
        auto last = line.end();
        --last;
        return Text_iterator(last, last->end());
    }
};

std::istream& operator>>(std::istream& is, Document& d) {
    for (char ch; is.get(ch);) {
        d.line.back().push_back(ch);
        if (ch == '\n') {
            d.line.emplace_back();
        }
    }
    if (d.line.back().size()) {
        d.line.emplace_back();
    }
    return is;
}

Text_iterator& Text_iterator::operator++() {
    ++pos;
    if (pos == ln->end()) {
        ++ln;
        pos = ln->begin();
    }
    return *this;
}

bool match(Text_iterator first, Text_iterator last, const std::string& s) {
    auto s_begin = s.begin();
    while (first != last && s_begin != s.end()) {
        if (*first != *s_begin) return false;
        ++first;
        ++s_begin;
    }
    return true;
}

template <typename Iter>
void advance(Iter& p, int n) {
    while (0 < n) {
        ++p;
        --n;
    }
}

void find_and_replace(Text_iterator first, Text_iterator last, const std::string& s, const std::string& rep) {
    if (s.empty() || rep.size() != s.size()) return;
    char first_char = s[0];
    while (true) {
        auto p = std::find(first, last, first_char);
        if (p == last) {
            return;
        }
        if (match(p, last, s)) {
            auto it = p;
            for (size_t i = 0; i < rep.size(); i++) {
                *it = rep[i];
                ++it;
            }
        }
    }

}

int main() {

}