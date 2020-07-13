#include <chrono>
#include <string>

struct Fruit {
    std::string name;
    int count;
    double unit_price;
    std::chrono::year_month_day last_sale_date;
};

struct Fruit_comparison {
    bool operator()(const Fruit* a, const Fruit* b) const
    {
        return a->name < b->name;
    }
};

int main() {

}