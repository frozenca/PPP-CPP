#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> v;
    for (std::string i; std::cin >> i;) {
        v.push_back(i);
    }
    std::sort(v.begin(), v.end());
    std::cout << "Max value is " << v.back() << '\n';
    std::cout << "Min value is " << v.front() << '\n';
    std::string mode;
    int freq = 0, max_freq = 0;
    for (size_t i = 0; i < v.size(); i++) {
        if (i == 0 || v[i] != v[i-1]) {
            freq = 1;
        } else {
            freq++;
        }
        if (max_freq < freq) {
            max_freq = freq;
            mode = v[i];
        }
    }
    std::cout << "Mode is " << mode << '\n';


}