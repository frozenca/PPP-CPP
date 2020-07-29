#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <set>
#include <regex>

using LineIter = std::vector<std::string>::const_iterator;

class Message {
    LineIter first;
    LineIter last;
public:
    Message(LineIter p1, LineIter p2) : first {p1}, last {p2} {}
    LineIter begin() const {return first;}
    LineIter end() const {return last;}

};

using MessIter = std::vector<Message>::const_iterator;

struct MailFile {
    std::string name;
    std::vector<std::string> lines;
    std::vector<Message> m;

    MailFile(const std::string& n);

    MessIter begin() const { return m.begin();}
    MessIter end() const { return m.end();}
};

std::regex addr_regex("^From: ");

bool findFromAddr(const Message* m, std::string& s) {
    std::smatch addr_match;
    for (const auto& x : *m) {
        if (std::regex_search(x, addr_match, addr_regex)) {
            s = addr_match.suffix();
            return true;
        }
    }
    return false;
}

std::regex subj_regex("^Subject: ");

std::string findSubject(const Message* m) {
    std::smatch subj_match;
    for (const auto& x : *m) {
        if (std::regex_search(x, subj_match, subj_regex)) {
            std::string s = subj_match.suffix();
            return s;
        }
    }
    return "";
}

MailFile::MailFile(const std::string& n) {
    std::ifstream in {n};
    if (!in) {
        std::cerr << "no " << n << '\n';
        throw std::runtime_error("No file");
    }

    for (std::string s; std::getline(in, s);) {
        lines.push_back(s);
    }
    if (lines.empty()) {
        throw std::runtime_error("empty message");
    }

    auto first = lines.begin();
    int num_from = 0, num_to = 0;
    for (auto p = lines.begin(); p != lines.end(); ++p) {
        if (p->find("––––") != std::string::npos) {
            m.emplace_back(first, p);
            first = p + 1;
            num_from = 0;
            num_to = 0;
        }
    }

    std::set<std::pair<std::string, std::string>> st;
    for (const auto& x : m) {
        std::string s;
        findFromAddr(&x, s);
        auto sj = findSubject(&x);
        if (!sj.empty()) {
            st.insert({s, findSubject(&x)});
        }
    }
    if (st.size() != std::count_if(m.begin(), m.end(), [](auto x){return !findSubject(&x).empty();})) {
        throw std::runtime_error("messages with same addresses and same subject");
    }
}

int main() {
    MailFile mfile {"my-mail-file.txt"};
    std::unordered_multimap<std::string, const Message*> sender;
    for (const auto& m : mfile) {
        std::string s;
        if (findFromAddr(&m, s)) {
            s.pop_back(); // Important!
            sender.emplace(s, &m);
        }
    }

    auto [p1, p2] = sender.equal_range("John Doe <jdoe@machine.example>");

    for (auto p = p1; p != p2; ++p) {
        std::cout << findSubject(p->second) << '\n';
    }

}