#include "csp.h"
#include "prettyprint.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using Grid = std::vector<std::vector<char>>;

struct Location {
    Location(int row, int col): row(row), col(col) {}
    const int row;
    const int col;
};

bool operator==(const Location& lhs, const Location& rhs) {
    return lhs.row == rhs.row && lhs.col == rhs.col;
}

bool operator<(const Location& lhs, const Location& rhs) {
    return lhs.row < rhs.row
        || (lhs.row == rhs.row && lhs.col < rhs.col);
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

std::vector<int> seq(int from, int to) {
    auto sz = to - from;
    std::vector<int> v(sz);
    for (int i = 0; i < sz; ++i) v[i] = from + i;
    return v;
}

std::vector<Location> zip(const std::vector<int>& rows, const std::vector<int>& cols) {
    std::vector<Location> v;
    auto size = rows.size();
    v.reserve(size);
    for (size_t i = 0; i < size; ++i) v.emplace_back(rows[i], cols[i]);
    return v;
}

std::vector<std::vector<Location>> generate_domain(const std::string& word, const Grid& grid) {
    std::vector<std::vector<Location>> domain;
    const auto height = grid.size();
    const auto width = grid[0].size();
    const auto word_length = word.size();
    for (size_t row = 0; row < height; ++row) {
        for (size_t col = 0; col < width; ++col) {
            auto columns = seq(col, col + word_length);
            auto rows = seq(row, row + word_length);
            if (col + word_length < width) { // left to right
                domain.push_back(zip(std::vector<int>(word_length, row), columns));
            }
            if (row + word_length < height) { // top to bottom
                domain.push_back(zip(rows, std::vector<int>(word_length, col)));
            }
            if (col + word_length < width && row + word_length < height) { // diagonal
                domain.push_back(zip(rows, columns));
                domain.push_back(zip(rows, {columns.crbegin(), columns.crend()}));
            }
        }
    }
    return domain;
}

struct WordSearchConstraint {
    std::vector<std::string> words;
    bool contains(const std::string&) { return true; }
    bool is_satisfied(const Assigment<std::string, std::vector<Location>>& a) const {
        std::vector<Location> locations;
        for (const auto& pair: a) {
            std::copy(
                    pair.second.cbegin(), pair.second.cend(),
                    std::back_inserter(locations)
                    );
        }
        std::set<Location> unique_locations{locations.cbegin(), locations.cend()};
        return unique_locations.size() == locations.size();
    }
};

int main() {
    auto grid = generate_grid(9, 9);
    std::vector<std::string> words{"MATTHEW", "JOE", "SARAH", "SALLY"};
    std::map<std::string, std::vector<std::vector<Location>>> locations;
    for (const auto& w: words) locations[w] = generate_domain(w, grid);
}
