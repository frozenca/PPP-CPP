#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

struct Reading {
    int hour;
    double temperature;
};

int main() {
    std::fstream fs {"raw_temps.txt"};
    if (!fs) {
        throw std::runtime_error("Can't open output file");
    }

    std::vector<Reading> readings;
    readings.reserve(50);
    for (int i = 0; i < 50; i++) {
        readings.push_back(Reading({i % 24, static_cast<double>(i)}));
    }
    for (const auto& reading : readings) {
        fs << reading.hour << ' ' << reading.temperature << ' ' << 'c' << '\n';
    }

    fs.seekg(0);
    fs.exceptions(fs.exceptions() | std::ios_base::badbit);
    std::vector<Reading> new_readings;
    int hour = 0;
    double temperature = 0.0;
    double total_temperature = 0.0;
    char c;
    while (fs >> hour >> temperature >> c) {
        if (c == 'c') {
            temperature = temperature * 9.0 / 5.0 + 32.0;
        } else if (c != 'f') {
            throw std::runtime_error("Invalid temperature");
        }
        new_readings.push_back(Reading({hour, temperature}));
        total_temperature += temperature;
    }
    std::sort(new_readings.begin(), new_readings.end(), [](auto& a, auto& b) {
        return a.temperature < b.temperature;
    });
    double average_temperature = total_temperature / new_readings.size();
    double median_temperature = (new_readings[new_readings.size() / 2].temperature +
                                 new_readings[(new_readings.size() + 1) / 2].temperature) / 2.0;
    std::cout << "Average: " << average_temperature
        << " Median: " << median_temperature;

}