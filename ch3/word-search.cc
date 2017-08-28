#include "prettyprint.hpp"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using Grid = std::vector<std::vector<char>>;

struct Location { int row, col; };

bool operator==(const Location& lhs, const Location& rhs) {
    return lhs.row == rhs.row && lhs.col == rhs.col;
}

const std::string ALPABHET{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

Grid generate_grid(int rows, int cols) {
    Grid g(rows);
    for (auto& row : g) {
        std::generate_n(std::back_inserter(row), cols, [](){
                return ALPABHET[rand() % ALPABHET.size()];
                });
    }
    return g;
}

int main() {
    std::cout << generate_grid(9, 9) << '\n';
}
