#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>
#include <unordered_map>
#include <fstream>

const char number = '8';    // t.kind==number means that t is a number Token
const char print = ';';    // t.kind==print means that t is a print Token
const char name = 'a';    // name token
const char let = 'L';    // declaration token
const char constant = 'C';
const char quit = 'q';
const char help = 'h';
const char from = 'f';
const char to = 't';
const std::string quitkey = "quit";
const std::string helpkey = "help";
const std::string declkey = "let";// declaration keyword
const std::string constkey = "const";// constant declaration keyword
const std::string prompt = "> ";
const std::string result = "= "; // used to indicate that what follows is a result
const std::string fromkey = "from";
const std::string tokey = "to";

std::ifstream ifs {"input.txt"};
std::ofstream ofs {"output.txt"};

// run-time checked narrowing cast (type conversion). See ???.
template<class R, class A>
R narrow_cast(const A &a) {
    R r = R(a);
    if (A(r) != a) throw std::runtime_error("info loss");
    return r;
}

std::unordered_map<char, size_t> rom2int = {{'I', 1},
                                            {'V', 5},
                                            {'X', 10},
                                            {'L', 50},
                                            {'C', 100},
                                            {'D', 500},
                                            {'M', 1000}};

class Roman_int {
public:
    std::string str;
    int value;
    Roman_int() : str{""}, value(0) {}
    Roman_int (std::string s, int v) : str{std::move(s)}, value{v}{
    }
    [[nodiscard]] int as_int() const {
        return value;
    }
};

std::istream& operator>>(std::istream& is, Roman_int& rm) {
    char ch;
    is >> ch;
    if (ch != '{') {
        is.unget();
        is.clear(std::ios::failbit);
        return is;
    }
    std::string sv;
    is >> sv;
    if (sv.empty()) throw std::runtime_error("Empty string");
    size_t sum = rom2int[sv.back()];
    if (!sum) throw std::runtime_error("invalid character!");
    rm.str = sv;
    if (sv.size() == 1) {
        rm.value = sum;
        return is;
    }
    for (size_t i = sv.size() - 2; i < sv.size(); i--) {
        if (rom2int[sv[i]] < rom2int[sv[i + 1]]) {
            if (rom2int[sv[i]] == rom2int[sv[i + 1]] / 5 ||
                rom2int[sv[i]] == rom2int[sv[i + 1]] / 10) {
                sum -= rom2int[sv[i]];
            } else {
                throw std::invalid_argument("invalid roman number!");
            }
        } else {
            sum += rom2int[sv[i]];
        }
    }
    rm.value = sum;
    return is;
}

std::ostream& operator<<(std::ostream& os, const Roman_int& rm) {
    return os << rm.as_int() << ' ';
}

class Token {
public:
    char kind;        // what kind of token
    Roman_int value;     // for numbers: a value
    std::string name;      // for names: name itself
    explicit Token(char ch) : kind(ch), value(Roman_int{}) {}

    Token(char ch, Roman_int& rm) : kind{ch}, value{rm} {}
    Token(char ch, std::string n) : kind(ch), name(std::move(n)), value(Roman_int{}) {}
};

//------------------------------------------------------------------------------

class Token_stream {
public:
    Token_stream();   // make a Token_stream that reads from ifs
    Token get();      // get a Token (get() is defined elsewhere)
    void putback(Token t);    // put a Token back
    void ignore(char c);      // discard tokens up to an including a c
private:
    bool full;        // is there a Token in the buffer?
    Token buffer;     // here is where we keep a Token put back using putback()
};

//------------------------------------------------------------------------------

// The constructor just sets full to indicate that the buffer is empty:
Token_stream::Token_stream()
        : full(false), buffer(0)    // no Token in buffer
{
}

//------------------------------------------------------------------------------

// The putback() member function puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token t) {
    if (full) throw std::runtime_error("putback() into a full buffer");
    buffer = std::move(t);       // copy t to buffer
    full = true;      // buffer is now full
}

//------------------------------------------------------------------------------

Token Token_stream::get() // read characters from ifs and compose a Token
{
    if (full) {         // check if we already have a Token ready
        full = false;
        return buffer;
    }

    char ch;
    ifs >> ch;          // note that >> skips whitespace (space, newline, tab, etc.)

    switch (ch) {
        case print:
        case '(':
        case ')':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=':
            return Token(ch); // let each character represent itself
        case '.':             // a floating-point literal can start with a dot
        case 'I':
        case 'V':
        case 'X':
        case 'L':
        case 'C':
        case 'D':
        case 'M':    // numeric literal
        {
            ifs.putback(ch);// put digit back into the input stream
            Roman_int rm;
            ifs >> rm;     // read a floating-point number
            return Token(number, rm);
        }
        case '\n': {
            return Token(print);
        }
        default:
            if (isalpha(ch)) {
                std::string s;
                s += ch;
                while (ifs.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
                ifs.putback(ch);
                if (s == declkey) return Token(let); // keyword "let"
                else if (s == constkey) return Token(constant); // keyword "constant"
                else if (s == quitkey) return Token(quit);
                else if (s == helpkey) return Token(help);
                else if (s == fromkey) return Token(from);
                else if (s == tokey) return Token(to);
                return Token(name, s);
            }
            throw std::runtime_error("Bad token");
    }
}

//------------------------------------------------------------------------------

void Token_stream::ignore(char c) {
// c represents the kind of a Token
    // first look in buffer:
    if (full && c == buffer.kind) {
        full = false;
        return;
    }
    full = false;

    // now search input:
    char ch = 0;
    while (ifs >> ch)
        if (ch == c) return;
}

//------------------------------------------------------------------------------

Token_stream ts;        // provides get() and putback()

//------------------------------------------------------------------------------

class Variable {
public:
    std::string name;
    double value;
    bool constant;

    Variable(std::string n, double v) : name(std::move(n)), value(v), constant(false) {}
    Variable(std::string n, double v, bool c) : Variable(std::move(n), v) { constant = c; }
};

//------------------------------------------------------------------------------
class Symbol_table {
public:
    std::vector<Variable> var_table;
    double get(const std::string &s);
    void set(const std::string &s, double d);
    bool is_declared(const std::string &var);
    double define_name(const std::string &var, double val, bool is_constant = false);

};

Symbol_table st;

//------------------------------------------------------------------------------

double Symbol_table::get(const std::string &s)
// return the value of the Variable names s
{
    for (const auto &stored_var : var_table)
        if (stored_var.name == s) return stored_var.value;
    throw std::runtime_error("get: undefined variable ");
}

//------------------------------------------------------------------------------

void Symbol_table::set(const std::string &s, double d) {
// set the Variable named s to d
    for (auto &stored_var : var_table)
        if (stored_var.name == s) {
            if (stored_var.constant) {
                throw std::runtime_error("set: you can't change the value of the constant");
            }
            stored_var.value = d;
            return;
        }
    throw std::runtime_error("set: undefined variable");
}

//------------------------------------------------------------------------------

bool Symbol_table::is_declared(const std::string &var) {
// is var already in var_table?
    for (const auto &stored_var : var_table)
        if (stored_var.name == var) return true;
    return false;
}

//------------------------------------------------------------------------------

double Symbol_table::define_name(const std::string &var, double val, bool is_constant) {
// add (var,val) to var_table
    if (is_declared(var)) {
        set(var, val);
    }
    var_table.emplace_back(var, val, is_constant);
    return val;
}

//------------------------------------------------------------------------------

double expression();    // declaration so that primary() can call expression()

//------------------------------------------------------------------------------

// deal with numbers and parentheses
double primary() {
    Token t = ts.get();
    switch (t.kind) {
        case '(':           // handle '(' expression ')'
        {
            double d = expression();
            t = ts.get();
            if (t.kind != ')') throw std::runtime_error("')' expected");
            return d;
        }
        case number:
            return t.value.value;    // return the number's value
        case name:
            return st.get(t.name); // return the variable's value
        case '-':
            return -primary();
        case '+':
            return primary();
        default:
            throw std::runtime_error("primary expected");
    }
}

// deal with *, /, and %
double term() {
    double left = primary();
    Token t = ts.get(); // get the next token from token stream

    while (true) {
        switch (t.kind) {
            case '*':
                left *= primary();
                t = ts.get();
                break;
            case '/': {
                double d = primary();
                if (d == 0) throw std::runtime_error("divide by zero");
                left /= d;
                t = ts.get();
                break;
            }
            case '%': {
                int i1 = narrow_cast<int>(left);
                int i2 = narrow_cast<int>(term());
                if (i2 == 0) throw std::runtime_error("%: divide by zero");
                left = i1 % i2;
                t = ts.get();
                break;
            }
            default:
                ts.putback(t);        // put t back into the token stream
                return left;
        }
    }
}

// deal with + and -
double expression() {
    double left = term();      // read and evaluate a Term
    Token t = ts.get();        // get the next token from token stream

    while (true) {
        switch (t.kind) {
            case '+':
                left += term();    // evaluate Term and add
                t = ts.get();
                break;
            case '-':
                left -= term();    // evaluate Term and subtract
                t = ts.get();
                break;
            default:
                ts.putback(t);     // put t back into the token stream
                return left;       // finally: no more + or -: return the answer
        }
    }
}

//------------------------------------------------------------------------------

double declaration(bool is_constant = false)
// handle: name = expression
// declare a variable called "name" with the initial value "expression"
{
    Token t = ts.get();
    if (t.kind != name) throw std::runtime_error("name expected in declaration");
    std::string var_name = t.name;

    Token t2 = ts.get();
    if (t2.kind != '=') throw std::runtime_error("= missing in declaration of var");

    double d = expression();
    st.define_name(var_name, d, is_constant);
    return d;
}

double statement() {
    Token t = ts.get();
    switch (t.kind) {
        case let:
            return declaration();
        case constant: {
            Token t2 = ts.get();
            if (t2.kind == let) {
                return declaration(true);
            } else {
                throw std::runtime_error("error: const should be followed by let");
            }
        }
        case from: {
            Token t2 = ts.get();
            ifs = std::ifstream {t2.name};
            return expression();
        }
        case to: {
            Token t2 = ts.get();
            ofs = std::ofstream {t2.name};
            return expression();
        }
        default:
            ts.putback(t);
            return expression();
    }
}

void clean_up_mess() {
    ts.ignore(print);
}

void calculate() {
    while (ifs) {
        try {
            ofs << prompt;
            Token t = ts.get();
            while (t.kind == print) t = ts.get();    // first discard all "prints"
            if (t.kind == quit) return;
            if (t.kind == help) {
                ofs << "Calculator: supports +, -, %, /, %, ()\n";
                ofs << "'quit' for quit\n";
                ofs << "'help' for help\n";
                ofs << "'let': declaration or assignment of variables\n";
                ofs << "'const let': declaration of constant variables\n";
            }
            ts.putback(t);
            ofs << result << statement() << '\n';
        }
        catch (std::exception &e) {
            std::cerr << e.what() << '\n';        // write throw std::runtime_error message
            clean_up_mess();
        }
    }
}

//------------------------------------------------------------------------------

int main()
try {
    // predefine names:
    st.define_name("pi", 3.1415926535, true);
    st.define_name("e", 2.7182818284, true);

    calculate();
    return 0;
}
catch (std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
}
catch (...) {
    std::cerr << "exception \n";
    return 2;
}

//------------------------------------------------------------------------------
