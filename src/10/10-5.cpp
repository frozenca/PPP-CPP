#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

const int not_a_reading = -7777;    // less than absolute zero
const int not_a_month = -1;

struct Day {
    std::vector<double> hour;
    Day();
};

Day::Day() : hour (24) {
    for (double& i : hour) i = not_a_reading;
}

//------------------------------------------------------------------------------

struct Month {        // a month of temperature readings
    int month;        // [0:11] January is 0
    std::vector<Day> day;  // [1:31] one vector of readings per day
    Month() : month{not_a_month}, day(32) { } // at most 31 days in a month (day[0] wasted)
};

//------------------------------------------------------------------------------

struct Year {             // a year of temperature readings, organized by month
    int year;             // positive == A.D.
    std::vector<Month> month;  // [0:11] January is 0
    Year() :month(12) { } // 12 months in a year
};

struct Reading {
    int day;
    int hour;
    double temperature;
};

int month_to_int(const std::string& s);
bool is_valid(const Reading& r);
void end_of_loop(std::istream& ist, char term, const std::string& message);

std::istream &operator>>(std::istream& is, Reading& r) {
// read a temperature reading from is into r
// format: ( 3 4 9.7 )
// check format, but don't bother with data validity
    char ch1;
    if (is >> ch1 && ch1 != '(') {    // could it be a Reading?
        is.unget();
        is.clear(std::ios_base::failbit);
        return is;
    }

    char ch2;
    int d = 0, h = 0;
    double t = 0.0;
    is >> d >> h >> t >> ch2;
    if (!is || ch2!=')') throw std::runtime_error("bad reading"); // messed up reading
    r.day = d;
    r.hour = h;
    r.temperature = t;
    return is;
}

std::istream& operator>>(std::istream& is, Month& m) {
// read a month from is into m
// format: { month feb ... }
    char ch = 0;
    if (is >> ch && ch != '{') {
        is.unget();
        is.clear(std::ios_base::failbit);    // we failed to read a Month
        return is;
    }

    std::string month_marker;
    std::string mm;
    is >> month_marker >> mm;
    if (!is || month_marker!="month") throw std::runtime_error("bad start of month");
    m.month = month_to_int(mm);

    Reading r;
    int no_of_duplicate_readings = 0;
    int no_invalid_readings = 0;

    while (is >> r) {
        if (is_valid(r)) {
            if (m.day[r.day].hour[r.hour] != not_a_reading)
                ++no_of_duplicate_readings;
            m.day[r.day].hour[r.hour] = r.temperature;
        } else {
            ++no_invalid_readings;
        }
    }
    end_of_loop(is,'}',"bad end of month");
    return is;
}

const int implausible_min = -200;
const int implausible_max = 200;

bool is_valid(const Reading& r) {
// a rough test
    if (r.day<1 || 31<r.day) return false;
    if (r.hour<0 || 23<r.hour) return false;
    return !(r.temperature < implausible_min || implausible_max < r.temperature);
}

//------------------------------------------------------------------------------

std::istream& operator>>(std::istream& is, Year& y) {
// read a year from is into y
// format: { year 1972 ... }
    char ch;
    is >> ch;
    if (ch != '{') {
        is.unget();
        is.clear(std::ios::failbit);
        return is;
    }

    std::string year_marker;
    int yy = 0;
    is >> year_marker >> yy;
    if (!is || year_marker!="year") throw std::runtime_error("bad start of year");
    y.year = yy;

    while (true) {
        Month m;    // get a clean m each time around
        if (!(is >> m)) break;
        y.month[m.month] = m;
    }

    end_of_loop(is,'}',"bad end of year");
    return is;
}

//------------------------------------------------------------------------------

void end_of_loop(std::istream& ist, char term, const std::string& message) {
    if (ist.fail()) { // use term as terminator and/or separator
        ist.clear();
        char ch;
        if (ist >> ch && ch == term) return;    // all is fine
        throw std::runtime_error(message);
    }
}


std::vector<std::string> month_input_tbl;    // month_input_tbl[0]=="jan"

void init_input_tbl(std::vector<std::string>& tbl)
// initialize vector of input representations
{
    tbl.emplace_back("jan");
    tbl.emplace_back("feb");
    tbl.emplace_back("mar");
    tbl.emplace_back("apr");
    tbl.emplace_back("may");
    tbl.emplace_back("jun");
    tbl.emplace_back("jul");
    tbl.emplace_back("aug");
    tbl.emplace_back("sep");
    tbl.emplace_back("oct");
    tbl.emplace_back("nov");
    tbl.emplace_back("dec");
}

int month_to_int(const std::string& s) {
// is s the name of a month? If so return its index [0:11] otherwise -1
    for (int i = 0; i < 12; ++i) if (month_input_tbl[i] == s) return i;
    return -1;
}

//------------------------------------------------------------------------------

std::vector<std::string> month_print_tbl;    // month_print_tbl[0]=="January"

void init_print_tbl(std::vector<std::string>& tbl) {
// initialize vector of output representations
    tbl.emplace_back("January");
    tbl.emplace_back("February");
    tbl.emplace_back("March");
    tbl.emplace_back("April");
    tbl.emplace_back("May");
    tbl.emplace_back("June");
    tbl.emplace_back("July");
    tbl.emplace_back("August");
    tbl.emplace_back("September");
    tbl.emplace_back("October");
    tbl.emplace_back("November");
    tbl.emplace_back("December");
}

std::string int_to_month(int i) {
// months [0:11]
    if (i < 0 || 12 <= i) throw std::runtime_error("bad month index");
    return month_print_tbl[i];
}

void print_year(std::ostream& ost, const Year& y) {
    ost << y.year << ": ";
    for (const auto& month : y.month) {
        ost << month.month << ": ";
        for (const auto& day : month.day) {
            for (auto hour : day.hour) {
                ost << hour << ' ';
            }
            ost << '\n';
        }
        ost << '\n';
    }
    ost << '\n';
}

int main() {
    try {
        // first initialize representation tables:
        init_print_tbl(month_print_tbl);
        init_input_tbl(month_input_tbl);

        // open an input file:
        std::cout << "Please enter input file name\n";
        std::string name;
        std::cin >> name;
        std::ifstream ifs {name};
        if (!ifs) throw std::runtime_error("can't open input file");

        ifs.exceptions(ifs.exceptions() | std::ios_base::badbit);    // throw for bad()

        // open an output file:
        std::cout << "Please enter output file name\n";
        std::cin >> name;
        std::ofstream ofs {name};
        if (!ofs) throw std::runtime_error("can't open output file");

        // read an arbitrary number of years:
        std::vector <Year> ys;
        while (true) {
            Year y;        // get a freshly initialized Year each  time around
            if (!(ifs >> y)) break;
            ys.emplace_back(y);
        }
        std::cout << "read " << ys.size() << " years of readings\n";

        for (const auto& y : ys) print_year(ofs, y);
    }
    catch (std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "Oops: unknown exception!\n";
        return 2;
    }

}
