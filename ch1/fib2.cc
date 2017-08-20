#include <iostream>
#include <string>

unsigned fib(unsigned n) {
    return n < 2
        ? n
        : fib(n - 2) + fib(n - 1);
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::cout << fib(std::stoi(argv[i])) << '\n';
    }
}
