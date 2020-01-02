
#include <iostream>


class Date {
public:
    Date(long int d);       // check for valid date and initialize
    Date();                            // default constructor
    // the default copy operations are fine

    // non-modifying operations:
    [[nodiscard]] long int day() const { return d; }

    // modifying operations:
    void add_day(int n);

private:
    long int d;
};

bool is_date(long int d); // true for valid date

bool leapyear(int y);                  // true if y is a leap year

bool operator==(const Date &a, const Date &b);

bool operator!=(const Date &a, const Date &b);

std::ostream &operator<<(std::ostream &os, const Date &d);

std::istream &operator>>(std::istream &is, Date &dd);

Date::Date(long int dd) : d{dd} {
    if (!is_date(d)) throw std::runtime_error("Invalid date");
}

const Date &default_date() {
    static const Date dd {0}; // start of 21st century
    return dd;
}

Date::Date() : d(default_date().day()) {
}

void Date::add_day(int n) {
    d += n;
}

bool is_date(long int d) {
    return d >= 0;
}

bool leapyear(int y) {
    // See exercise ???
    return false;
}

bool operator==(const Date &a, const Date &b) {
    return a.day() == b.day();
}

bool operator!=(const Date &a, const Date &b) {
    return !(a == b);
}

std::ostream &operator<<(std::ostream &os, const Date &d) {
    return os << '(' << d.day()
              << ')';
}

std::istream &operator>>(std::istream &is, Date &dd) {
    int d = 0;
    char ch1, ch2;
    is >> ch1 >> d >> ch2;
    if (!is) return is;
    if (ch1 != '(' || ch2 != ')') { // oops: format error
        is.clear(std::ios_base::failbit);                    // set the fail bit
        return is;
    }
    dd = Date(d);     // update dd
    return is;
}

enum Day {
    sunday, monday, tuesday, wednesday, thursday, friday, saturday
};

Day day_of_week(const Date &d) {
    return sunday;
}

Date next_Sunday(const Date &d) {
    return d;
}

Date next_weekday(const Date &d) {
    return d;
}


int main() {

}