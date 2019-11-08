#include <algorithm>
#include <iostream>
#include <vector>

// compute mean and median temperatures
int main() {
    std::vector<double> temps;                  // temperatures
    for (double temp; std::cin >> temp;)    // read into temp
        temps.push_back(temp);     // put temp into vector

    // compute mean temperature:
    double sum = 0;
    for (double x : temps) sum += x;
    std::cout << "Average temperature: " << sum / temps.size() << '\n';

    // compute median temperature:
    std::sort(temps.begin(), temps.end());                                     // sort temperatures
    std::cout << "Median temperature: " << ((temps.size() % 2) ? temps[temps.size() / 2] :
        (temps[temps.size() / 2] + temps[temps.size() / 2 - 1]) / 2.0) << '\n';
}