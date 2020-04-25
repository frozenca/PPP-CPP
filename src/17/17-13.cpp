#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <utility>

struct God {
    std::string name;
    std::string mythology;
    std::string vehicle;
    std::string weapon;

    God(std::string name, std::string mythology, std::string vehicle, std::string weapon) :
            name{std::move(name)}, mythology{std::move(mythology)}, vehicle{std::move(vehicle)},
            weapon{std::move(weapon)} {
    }
    bool operator<(const God& other) const {
        return name < other.name;
    }
};

std::ostream& operator<<(std::ostream& os, const God& god) {
    os << "Name : " << god.name << ", Mythology : " << god.mythology << ", Vehicle : " << god.vehicle << ", Weapon : "
       << god.vehicle << '\n';
    return os;
}

int main() {
    std::list<God> gods;
    gods.emplace_back("Zeus", "Greek", "", "lightning");
    gods.emplace_back("Odin", "Norse", "Eight-legged flying horse called Sleipner", "Spear called Gungnir");
    for (const auto& god : gods) {
        std::cout << god;
    }
    gods.sort();
    God new_god {"Thor", "Norse", "", "Hammer"};
    auto it = std::find_if_not(gods.begin(), gods.end(), [new_god](const auto& god) {
        return god.name < new_god.name;
    });
    gods.insert(it, new_god);
    for (const auto& god : gods) {
        std::cout << god;
    }

}