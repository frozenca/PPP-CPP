#include <cassert>

struct Tracer {
    Tracer() {
        std::cout << "Default constructor\n";
    }

    Tracer(const Tracer&) {
        std::cout << "Copy constructor\n";
    }

    Tracer& operator=(const Tracer&) {
        std::cout << "Copy assignment operator\n";
        return *this;
    }

    Tracer(Tracer&&) noexcept {
        std::cout << "Move constructor\n";
    }

    Tracer& operator=(Tracer&&) noexcept {
        std::cout << "Move assignment operator\n";
        return *this;
    }

    ~Tracer() {
        std::cout << "Destructor\n";
    }
};

int main() {
}