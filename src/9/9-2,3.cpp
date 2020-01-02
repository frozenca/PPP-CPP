#include <iostream>
#include <vector>
#include <string>
#include <utility>

class Name_pairs {
public:
    std::vector<std::string> name;
    std::vector<double> age;

    void read_names(std::istream& is);
    void read_ages(std::istream& is);
    void sort();
};

void Name_pairs::read_names(std::istream& is) {
    std::string s;
    while (is >> s) {
        if (s == "q") {
            break;
        }
        name.push_back(s);
    }
}

void Name_pairs::read_ages(std::istream& is) {
    double d = 0.0;
    while (is >> d) {
        if (age.size() == name.size()) {
            break;
        }
        age.push_back(d);
    }
}

std::ostream& operator<<(std::ostream& os, const Name_pairs& np) {
    for (size_t i = 0; i < np.name.size(); i++) {
        os << np.name[i] << ' ' << np.age[i] << '\n';
    }
    return os;
}

void Name_pairs::sort() {
    // insertion sort
    for (size_t i = 0; i < name.size(); i++) {
        size_t j = i;
        while (j > 0 && name[j - 1] > name[j]) {
            std::swap(name[j], name[j - 1]);
            std::swap(age[j], age[j - 1]);
        }
    }
}

int main() {
    Name_pairs np;
    np.read_names(std::cin);
    np.read_ages(std::cin);
    std::cout << np;
    np.sort();
    std::cout << np;
}