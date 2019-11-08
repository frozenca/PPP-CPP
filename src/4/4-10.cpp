#include <algorithm>
#include <iostream>
#include <random>

int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> rps(0, 2);
    std::cout << "Give Rock, Paper or Scissors (r), (p), (s):\n";
    char you;
    std::cin >> you;
    int com = rps(gen);
    switch (you) {
        case 'r':
            if (com == 0) {
                std::cout << "Computer rock: draw\n";
            } else if (com == 1) {
                std::cout << "Computer paper: you lose!\n";
            } else {
                std::cout << "Computer scissor: you win!\n";
            }
            break;
        case 'p':
            if (com == 0) {
                std::cout << "Computer rock: you win!\n";
            } else if (com == 1) {
                std::cout << "Computer paper: draw\n";
            } else {
                std::cout << "Computer scissor: you lose!\n";
            }
            break;
        case 's':
            if (com == 0) {
                std::cout << "Computer rock: you lose!\n";
            } else if (com == 1) {
                std::cout << "Computer paper: you win!\n";
            } else {
                std::cout << "Computer scissor: draw\n";
            }
            break;
        default:
            std::cout << "Sorry, I don't know your move\n";
    }

}