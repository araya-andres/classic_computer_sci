#include <iostream>
#include <unordered_map>
#include <string>

unsigned fib(unsigned n) {
    static std::unordered_map<unsigned, unsigned> memo{{0, 0}, {1, 1}};
    auto it = memo.find(n);
    if (it != memo.end()) {
        return it->second;
    }
    const auto ans = fib(n - 2) + fib(n - 1);
    memo[n] = ans;
    return ans;
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::cout << fib(std::stoi(argv[i])) << '\n';
    }
}
