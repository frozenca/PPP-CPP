#include <iostream>
#include <ratio>

int main() {
    using a = std::ratio<3, 2>;
    using b = std::ratio<7, 5>;
    using c = std::ratio_add<a, b>;
    using d = std::ratio_subtract<a, b>;
    using e = std::ratio_multiply<a, b>;
    using f = std::ratio_divide<a, b>;
    double g = static_cast<double>(a::num) / a::den;
    std::cout << a::num << '/' << a::den << '\n';
    std::cout << b::num << '/' << b::den << '\n';
    std::cout << c::num << '/' << c::den << '\n';
    std::cout << d::num << '/' << d::den << '\n';
    std::cout << e::num << '/' << e::den << '\n';
    std::cout << f::num << '/' << f::den << '\n';
    std::cout << g;

}