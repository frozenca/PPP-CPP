#include <iostream>
#include <string>

class Money {
public:
    long int cents;
    Money(const std::string& in) {
        size_t n = in.find('.');
        if (n == std::string::npos) {
            cents = std::stoi(in.substr(1)) * 100;
        } else {
            cents = std::stoi(in.substr(1, n - 1)) * 100;
            size_t c = std::stoi(in.substr(n + 1));
            if (c % 5) {
                c += 5 - c % 5;
            }
            cents += c;
        }
    }
};

std::istream& operator>>(std::istream& istr, Money& money) {
    std::string s;
    istr >> s;
    if (s.size() < 4) {
        throw std::runtime_error("invalid string");
    }
    money = Money(s.substr(3));
    return istr;
}

std::ostream& operator<<(std::ostream& ostr, const Money& money) {
    ostr << "USD" << std::to_string(static_cast<double>(money.cents) / 100.0);
    return ostr;
}

int main() {

}