#include <algorithm>
#include <vector>
#include <iostream>
#include <list>
#include <iterator>
#include <string>
#include <cctype>

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

size_t num_words(Document& doc) {
    size_t nw = 0;
    bool ws = true;
    for (char ch : doc) {
        if (std::isblank(ch)) {
            if (!ws) {
                ws = true;
            }
        } else {
            if (ws) {
                ws = false;
                nw++;
            }
        }
    }
    return nw;
}

int main() {

}