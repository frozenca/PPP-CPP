#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>
#include <numeric>
#include <list>
#include <string>

struct Purchase {
    std::string name;
    double unit_price = 0.0;
    size_t count = 0;

    Purchase() = default;

    Purchase(std::string name, double unit_price, size_t count) : name{std::move(name)}, unit_price{unit_price},
                                                                  count{count} {}

    [[nodiscard]] double getValue() const {
        return unit_price * count;
    }
};

struct Order {
    std::string customer_name;
    std::string customer_address;
    std::vector<Purchase> purchases;

    [[nodiscard]] double totalValue() {
        double value = 0.0;
        for (const auto& purchase : purchases) {
            value += purchase.getValue();
        }
        return value;
    };
};

struct OrderCompByName {
    bool operator()(const Order& l, const Order& r) {
        return l.customer_name < r.customer_name;
    }
};

struct OrderCompByAddr {
    bool operator()(const Order& l, const Order& r) {
        return l.customer_address < r.customer_address;
    }
};

std::istream& operator>>(std::istream& is, Purchase& purchase) {
    std::string name;
    double unit_price;
    size_t count;

    is >> name >> unit_price >> count;
    if (!is) return is;
    purchase = Purchase {name, unit_price, count};

    return is;
}

std::ostream& operator<<(std::ostream& os, const Purchase& purchase) {
    os << purchase.name << ' ' << purchase.unit_price << ' ' << purchase.count << '\n';
    return os;
}

std::istream& operator>>(std::istream& is, Order& order) {
    std::string name, addr;
    is >> name >> addr;
    if (!is) return is;
    order.customer_name = name;
    order.customer_address = addr;
    for (Purchase purchase; is >> purchase; ) {
        order.purchases.push_back(purchase);
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << order.customer_name << ' ' << order.customer_address << '\n';
    for (const auto& purchase : order.purchases) {
        os << purchase;
    }
    return os;
}

int main() {
}