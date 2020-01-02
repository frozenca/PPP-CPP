#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <utility>
#include <cmath>

class Book {
private:
    enum class Genre {
        fiction, nonfiction, periodical, biography, children,
    };
    std::string ISBN;
    std::string title;
    std::string author;
    std::string copyright_date;
    bool is_checkedout;
    Genre genre;

    bool validate_ISBN() {
        return ISBN.size() == 7 && std::isdigit(ISBN[0]) && std::isdigit(ISBN[2]) && std::isdigit(ISBN[4])
               && (ISBN[1] == '-') && (ISBN[3] == '-') && (ISBN[5] == '-') &&
               (std::isdigit(ISBN[6]) || std::isalpha(ISBN[6]));
    }

public:
    Book(std::string ISBN, std::string title, std::string author, std::string copyright_date, int genre)
            : ISBN{std::move(ISBN)}, title{std::move(title)}, author{std::move(author)},
              copyright_date{std::move(copyright_date)}, is_checkedout{false}, genre{genre} {
        if (!validate_ISBN()) throw std::runtime_error("Invalid ISBN code!");
    }

    [[nodiscard]] std::string getISBN() const {
        return ISBN;
    }

    void setISBN(std::string s) {
        ISBN = std::move(s);
    }

    [[nodiscard]] std::string getTitle() const {
        return title;
    }

    void setTitle(std::string s) {
        title = std::move(s);
    }

    [[nodiscard]] std::string getAuthor() const {
        return author;
    }

    void setAuthor(std::string s) {
        author = std::move(s);
    }

    [[nodiscard]] std::string getCopyrightDate() const {
        return copyright_date;
    }

    void setCopyrightDate(std::string s) {
        copyright_date = std::move(s);
    }

    [[nodiscard]] std::string getGenre() const {
        switch (genre) {
            case Genre::fiction:
                return "fiction";
            case Genre::nonfiction:
                return "nonfiction";
            case Genre::periodical:
                return "periodical";
            case Genre::biography:
                return "biography";
            case Genre::children:
                return "children";
            default:
                throw std::runtime_error("Unknown genre");
        }
    }

    void setGenre(int n) {
        genre = Genre{n};
    }

    [[nodiscard]] bool getCheckedOut() const {
        return is_checkedout;
    }

    void setCheckedOut(bool checkedout) {
        is_checkedout = checkedout;
    }
};

std::ostream &operator<<(std::ostream &os, const Book &book) {
    os << "ISBN : " << book.getISBN() << '\n'
       << "Title : " << book.getTitle() << '\n'
       << "Author : " << book.getAuthor() << '\n'
       << "Copyright Date : " << book.getCopyrightDate() << '\n'
       << "Genre : " << book.getGenre() << '\n';
    return os;
}

bool operator==(const Book &book1, const Book &book2) {
    return book1.getISBN() == book2.getISBN();
}

bool operator!=(const Book &book1, const Book &book2) {
    return !(book1 == book2);
}

class Patron {
private:
    std::string name;
    int card_no;
    double fee;
public:
    Patron(std::string name, int card_no, double fee) : name{std::move(name)}, card_no{card_no}, fee{fee} {
    }

    [[nodiscard]] std::string getName() const {
        return name;
    }

    void setName(std::string s) {
        name = std::move(s);
    }

    [[nodiscard]] int getCardNo() const {
        return card_no;
    }

    void setCardNo(int n) {
        card_no = n;
    }

    [[nodiscard]] double getFee() const {
        return fee;
    }

    void setFee(double d) {
        fee = d;
    }

    [[nodiscard]] bool isOweing() const {
        return std::fabs(fee) > 1e-5;
    }
};

enum Month {
    jan = 1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec
};

class Date {
public:
    Date(int y, Month m, int d);       // check for valid date and initialize
    Date();                            // default constructor
    // the default copy operations are fine

    // non-modifying operations:
    [[nodiscard]] int day() const { return d; }
    [[nodiscard]] Month month() const { return m; }
    [[nodiscard]] int year() const { return y; }

    // modifying operations:
    void add_day(int n);
    void add_month(int n);
    void add_year(int n);

private:
    int y;
    Month m;
    int d;
};

bool leapyear(int y) {
    return (!(y % 4) && (y % 100)) || !(y % 400);
}

Month operator++(Month &m)                       // prefix increment operator
{
    m = (m == dec) ? jan : Month(m + 1);             // "wrap around"
    return m;
}

bool is_date(int y, Month m, int d) {
    // assume that y is valid

    if (d <= 0) return false;            // d must be positive
    int days_in_month = 31;            // most months have 31 days
    switch (m) {
        case feb:                        // the length of February varies
            days_in_month = (leapyear(y)) ? 29 : 28;
            break;
        case apr:
        case jun:
        case sep:
        case nov:
            days_in_month = 30;                // the rest have 30 days
            break;
        case jan:
        case mar:
        case may:
        case jul:
        case aug:
        case oct:
        case dec:
            days_in_month = 31;                // the rest have 31 days
            break;
    }
    return days_in_month >= d;

}

Date::Date(int yy, Month mm, int dd) : y(yy), m(mm), d(dd) {
    if (!is_date(yy, mm, dd)) throw std::runtime_error("invalid date");
}

const Date &default_date() {
    static const Date dd(2001, jan, 1); // start of 21st century
    return dd;
}

Date::Date() : y(default_date().year()), m(default_date().month()), d(default_date().day()) {
}

void Date::add_day(int n) {
    d += n;
}

void Date::add_month(int n) {
    for (int i = 0; i < n; i++) {
        ++m;
    }
}

void Date::add_year(int n) {
    if (m == feb && d == 29 && !leapyear(y + n)) { // beware of leap years!
        m = mar;        // use March 1 instead of February 29
        d = 1;
    }
    y += n;
}

bool operator==(const Date &a, const Date &b) {
    return a.year() == b.year()
           && a.month() == b.month()
           && a.day() == b.day();
}

bool operator!=(const Date &a, const Date &b) {
    return !(a == b);
}

std::ostream &operator<<(std::ostream &os, const Date &d) {
    return os << '(' << d.year()
              << ',' << d.month()
              << ',' << d.day()
              << ')';
}

std::istream &operator>>(std::istream &is, Date &dd) {
    int y, m, d;
    char ch1, ch2, ch3, ch4;
    is >> ch1 >> y >> ch2 >> m >> ch3 >> d >> ch4;
    if (!is) return is;
    if (ch1 != '(' || ch2 != ',' || ch3 != ',' || ch4 != ')') { // oops: format error
        is.clear(std::ios_base::failbit);                    // set the fail bit
        return is;
    }
    dd = Date(y, Month(m), d);     // update dd
    return is;
}

enum Day {
    sunday, monday, tuesday, wednesday, thursday, friday, saturday
};

std::vector<int> days_in_month {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

Day day_of_week(const Date &d) {
    int nd = 0;
    for (int i = 0; i < d.month(); i++) {
        nd += days_in_month[i];
    }
    nd += d.day() - 1;
    return Day(sunday + nd % 7);
}
Date next_Sunday(const Date &d) {
    int nd = 0;
    for (int i = 0; i < d.month(); i++) {
        nd += days_in_month[i];
    }
    nd += d.day() - 1;
    Date d2 = d;
    d2.add_day(7 - nd % 7);
    return d2;
}
Date next_weekday(const Date &d) {
    int nd = 0;
    for (int i = 0; i < d.month(); i++) {
        nd += days_in_month[i];
    }
    nd += d.day() - 1;
    Date d2 = d;
    if (nd % 7 == 5 || nd % 7 == 6) {
        d2.add_day(8 - nd % 7);
        return d2;
    } else {
        d2.add_day(1);
        return d2;
    }
}

class Library {
private:
    std::vector<Book> book;
    std::vector<Patron> patron;

    struct Transaction {
        Book book;
        Patron patron;
        Date date;
        Transaction(Book b, Patron  p, const Date& d) : book{std::move(b)}, patron{std::move(p)}, date{d} {}
    };

    std::vector<Transaction> transaction;

public:
    void add_book(const Book& b);
    void add_patron(const Patron& p);
    void checkout(const Book& b, const Patron& p);
    [[nodiscard]] std::vector<std::string> oweing_patrons() const;
};

void Library::add_book(const Book& b) {
    book.push_back(b);
}

void Library::add_patron(const Patron& p) {
    patron.push_back(p);
}

void Library::checkout(const Book& b, const Patron& p) {
    if (std::find(book.begin(), book.end(), b) == book.end()
    || std::find(patron.begin(), patron.end(), p) == patron.end()
    || p.isOweing()) {
        throw std::runtime_error("Book/patron does not exist or patron is oweing a fee");
    }
    transaction.emplace_back(b, p, Date{});
}

std::vector<std::string> Library::oweing_patrons() const {
    std::vector<std::string> res;
    for (const auto& pat : patron) {
        if (pat.isOweing()) {
            res.push_back(pat.getName());
        }
    }
    return res;
}

int main() {
}