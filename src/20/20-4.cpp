#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <stdexcept>

constexpr size_t cnt = 5;

double* get_from_jack(size_t* count) {
    auto* data = new double[cnt];
    *count = cnt;
    return data;
}

std::vector<double>* get_from_jill() {
    auto data = new std::vector<double>;
    data->resize(cnt);
    return data;
}

template <typename Iterator>
Iterator high(Iterator first, Iterator last) {
    if (!first || first == last) {
        throw std::runtime_error("Null iterator or empty range");
    }
    Iterator high = first;
    for (Iterator p = first; p != last; ++p) {
        if (*high < *p) {
            high = p;
        }
    }
    return high;
}

void fct() {
    size_t jack_count = 0;
    auto jack_data = get_from_jack(&jack_count);
    auto jill_data = get_from_jill();

    double* jack_high = high(jack_data, jack_data + jack_count);
    auto& v = *jill_data;
    double* jill_high = high(&v[0], &v[0] + v.size());

    std::cout << "Jill's max: " << *jill_high << ", Jack's max: " << *jack_high << '\n';

    delete[] jack_data;
    delete jill_data;
}

int main() {
    fct();

}