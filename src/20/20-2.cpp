#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>

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

void fct() {
    size_t jack_count = 0;
    auto jack_data = get_from_jack(&jack_count);
    auto jill_data = get_from_jill();

    double h = std::numeric_limits<double>::lowest();
    double* jack_high = nullptr;
    double* jill_high = nullptr;

    for (size_t i = 0; i < jack_count; i++) {
        if (h < jack_data[i]) {
            jack_high = &jack_data[i];
            h = jack_data[i];
        }
    }

    h = std::numeric_limits<double>::lowest();
    for (double& i : *jill_data) {
        if (h < i) {
            jill_high = &i;
            h = i;
        }
    }

    std::cout << "Jill's max: " << *jill_high << ", Jack's max: " << *jack_high << '\n';

    delete[] jack_data;
    delete jill_data;
}

int main() {
    fct();

}