#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>

const char number = '8';    // t.kind==number means that t is a number Token
const char print = ';';    // t.kind==print means that t is a print Token
const char name = 'a';    // name token
const char let = 'L';    // declaration token
const char constant = 'C';
const char quit = 'q';
const char help = 'h';
const std::string quitkey = "quit";
const std::string helpkey = "help";
const std::string declkey = "let";// declaration keyword
const std::string constkey = "const";// constant declaration keyword
const std::string prompt = "> ";
const std::string result = "= "; // used to indicate that what follows is a result

// run-time checked narrowing cast (type conversion). See ???.
template <class R, class A> R narrow_cast(const A& a)
{
    R r = R(a);
    if (A(r)!=a) throw std::runtime_error("info loss");
    return r;
}

//------------------------------------------------------------------------------

class Token {
public:
    char kind;        // what kind of token
    int value;     // for numbers: a value
    std::string name;      // for names: name itself
    explicit Token(char ch) : kind(ch), value(0) {}

    Token(char ch, double val) : kind(ch), value(narrow_cast<int>(val)) {}

    Token(char ch, std::string n) : kind(ch), name(std::move(n)), value(0) {}
};

//------------------------------------------------------------------------------

class Token_stream {
public:
    Token_stream();   // make a Token_stream that reads from std::cin
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

Token Token_stream::get() // read characters from std::cin and compose a Token
{
    if (full) {         // check if we already have a Token ready
        full = false;
        return buffer;
    }

    char ch;
    std::cin >> ch;          // note that >> skips whitespace (space, newline, tab, etc.)

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
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':    // numeric literal
        {
            std::cin.putback(ch);// put digit back into the input stream
            double val;
            std::cin >> val;     // read a floating-point number
            return Token(number, val);
        }
        case '\n': {
            return Token(print);
        }
        default:
            if (isalpha(ch)) {
                std::string s;
                s += ch;
                while (std::cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
                std::cin.putback(ch);
                if (s == declkey) return Token(let); // keyword "let"
                else if (s == constkey) return Token(constant); // keyword "constant"
                else if (s == quitkey) return Token(quit);
                else if (s == helpkey) return Token(help);
                return Token(name, s);
            }
            throw std::runtime_error("Bad token");
    }
}

//------------------------------------------------------------------------------

void Token_stream::ignore(char c)
// c represents the kind of a Token
{
    // first look in buffer:
    if (full && c == buffer.kind) {
        full = false;
        return;
    }
    full = false;

    // now search input:
    char ch = 0;
    while (std::cin >> ch)
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
    Variable(std::string n, double v, bool c) : Variable(std::move(n), v) { constant = c;}
};

//------------------------------------------------------------------------------
class Symbol_table {
public:
    std::vector<Variable> var_table;
    double get(const std::string& s);
    void set(const std::string& s, double d);
    bool is_declared(const std::string& var);
    double define_name(const std::string& var, double val, bool is_constant = false);

};

Symbol_table st;

//------------------------------------------------------------------------------

double Symbol_table::get(const std::string& s)
// return the value of the Variable names s
{
    for (const auto& stored_var : var_table)
        if (stored_var.name == s) return stored_var.value;
    throw std::runtime_error("get: undefined variable ");
}

//------------------------------------------------------------------------------

void Symbol_table::set(const std::string& s, double d)
// set the Variable named s to d
{
    for (auto& stored_var : var_table)
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

bool Symbol_table::is_declared(const std::string& var)
// is var already in var_table?
{
    for (const auto& stored_var : var_table)
        if (stored_var.name == var) return true;
    return false;
}

//------------------------------------------------------------------------------

double Symbol_table::define_name(const std::string& var, double val, bool is_constant)
// add (var,val) to var_table
{
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
            return t.value;    // return the number's value
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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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
        default:
            ts.putback(t);
            return expression();
    }
}

//------------------------------------------------------------------------------

void clean_up_mess() {
    ts.ignore(print);
}

//------------------------------------------------------------------------------

void calculate() {
    while (std::cin) {
        try {
            std::cout << prompt;
            Token t = ts.get();
            while (t.kind == print) t = ts.get();    // first discard all "prints"
            if (t.kind == quit) return;
            if (t.kind == help) {
                std::cout << "Calculator: supports +, -, %, /, %, ()\n";
                std::cout << "'quit' for quit\n";
                std::cout << "'help' for help\n";
                std::cout << "'let': declaration or assignment of variables\n";
                std::cout << "'const let': declaration of constant variables\n";
            }
            ts.putback(t);
            std::cout << result << statement() << '\n';
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
