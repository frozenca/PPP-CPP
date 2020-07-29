#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <set>

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

size_t isPrefix(const std::string& s, const std::string& p) {
    if (std::string(s, 0, p.size()) == p) {
        return p.size();
    } else {
        return 0;
    }
}

bool findFromAddr(const Message* m, std::string& s) {
    for (const auto& x : *m) {
        if (auto n = isPrefix(x, "From: ")) {
            s = std::string(x, n);
            return true;
        }
    }
    return false;
}

std::string findSubject(const Message* m) {
    for (const auto& x : *m) {
        if (auto n = isPrefix(x, "Subject: ")) {
            return std::string(x, n);
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
        if (isPrefix(*p, "From: ")) {
            num_from++;
            if (num_from >= 2) {
                throw std::runtime_error("Double sender");
            }
        }
        if (isPrefix(*p, "To: ")) {
            num_to++;
            if (num_to >= 2) {
                throw std::runtime_error("Double receiver");
            }
        }

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
    std::unordered_multimap<std::string, const Message*> senders;
    for (const auto& m : mfile) {
        std::string s;
        findFromAddr(&m, s);
        if (!s.empty()) {
            s.pop_back();
            senders.emplace(s, &m);
        }
    }

    std::string s;
    if (std::cin >> s) {
        auto [msg1, msg2] = senders.equal_range(s);
        for (auto msg = msg1; msg != msg2; ++msg) {
            std::cout << findSubject(msg->second) << '\n';
        }
    }


}