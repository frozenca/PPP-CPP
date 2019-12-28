#include <iostream>
#include <vector>

int main() {
    std::vector<double> price {100, 200, 300};
    std::vector<double> weight {100, 200, 300};
    if (price.size() != weight.size()) {
        throw std::runtime_error("price, weight size does not match");
    }
    for (size_t i = 0; i < price.size(); i++) {
        std::cout << price[i] * weight[i] << ' ';
    }

}