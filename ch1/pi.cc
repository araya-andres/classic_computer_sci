#include <iostream>

double calculate_pi(unsigned n) {
    auto numerator = 4.0;
    auto denominator = 3.0;
    auto pi = 4.0;
    auto sign = -1;
    while (--n > 0) {
        pi += sign * numerator / denominator;
        denominator += 2.0;
        sign *= -1;
    }
    return pi;
}

int main(int argc, char* argv[]) {
    if (argc == 0) return 1;
    auto n = std::stoi(argv[1]);
    for (int i = 1; i <= n; i++) {
        std::cout << calculate_pi(i) << '\n';
    }
}
