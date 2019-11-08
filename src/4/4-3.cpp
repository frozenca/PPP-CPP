#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    std::vector<double> dists;
    for (double dist = 0.0; std::cin >> dist;)
        dists.push_back(dist);

    double sum = std::accumulate(dists.begin(), dists.end(), 0.0);
    std::cout << "Total dist " << sum << '\n';

    std::sort(dists.begin(), dists.end());
    std::cout << "Smallest dist " << dists.front() << '\n';
    std::cout << "Biggest dist " << dists.back() << '\n';
    std::cout << "mean dist " << sum / std::size(dists) << '\n';

}