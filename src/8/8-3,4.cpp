#include <iostream>
#include <vector>

void print(const std::vector<int>& v, std::string& str) {
    for (auto num : v) {
        str += std::to_string(num);
        str += ' ';
    }
}

void fibonacci(int x, int y, std::vector<int>& v, int n) {
    v.clear();
    if (n >= 2) {
        v.push_back(x);
        v.push_back(y);
    }
    for (int i = 2; i < n; i++) {
        v.push_back(v[i - 1] + v[i - 2]);
        if (v[i] < v[i - 1]) {
            break;
        }
    }
}

int main() {
    std::vector<int> v;
    fibonacci(1, 2, v, 50);
    std::string str;
    print(v, str);
    std::cout << str;
}