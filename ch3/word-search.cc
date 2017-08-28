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
    Location(int row, int col): row_(row), col_(col) {}
    int row() const { return row_; }
    int col() const { return col_; }
private:
    int row_;
    int col_;
};

bool operator==(const Location& lhs, const Location& rhs) {
    return lhs.row() == rhs.row() && lhs.col() == rhs.col();
}

bool operator<(const Location& lhs, const Location& rhs) {
    return lhs.row() < rhs.row()
        || (lhs.row() == rhs.row() && lhs.col() < rhs.col());
}

std::ostream& operator<<(std::ostream& os, const Location& l) {
    return os << '{' << l.row() << ',' << l.col() << '}';
}

using Locations = std::vector<Location>;

using Domain = std::vector<Locations>;

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

void print_grid(const Grid& g) {
    for (const auto& row: g) {
        for (const auto& col: row) {
            std::cout << col;
        }
        std::cout << '\n';
    }
}

std::vector<int> seq(int from, int to) {
    auto size = to - from;
    std::vector<int> v(size);
    for (int i = 0; i < size; ++i) v[i] = from + i;
    return v;
}

Locations zip(const std::vector<int>& rows, const std::vector<int>& cols) {
    auto size = rows.size();
    Locations v;
    v.reserve(size);
    for (size_t i = 0; i < size; ++i) v.emplace_back(rows[i], cols[i]);
    return v;
}

Domain generate_domain(const std::string& word, const Grid& grid) {
    Domain domain;
    const auto height = grid.size();
    const auto width = grid[0].size();
    const auto word_length = word.size();
    for (size_t row = 0; row < height; ++row) {
        for (size_t col = 0; col < width; ++col) {
            auto columns = seq(col, col + word_length);
            auto rows = seq(row, row + word_length);
            if (col + word_length <= width) { // left to right
                domain.push_back(zip(std::vector<int>(word_length, row), columns));
            }
            if (row + word_length <= height) { // top to bottom
                domain.push_back(zip(rows, std::vector<int>(word_length, col)));
            }
            if (col + word_length <= width && row + word_length <= height) { // diagonal
                domain.push_back(zip(rows, columns));
                domain.push_back(zip(rows, {columns.crbegin(), columns.crend()}));
            }
        }
    }
    return domain;
}

struct WordSearchConstraint {
    WordSearchConstraint(const std::string& w) : word(w) {}
    bool contains(const std::string& w) const { return word == w; }
    bool is_satisfied(const Assigment<std::string, Locations>& a) const {
        Locations locations;
        for (const auto& [_, l]: a) {
            std::copy(l.cbegin(), l.cend(), std::back_inserter(locations));
        }
        std::set<Location> unique_locations{locations.cbegin(), locations.cend()};
        return unique_locations.size() == locations.size();
    }
private:
    std::string word;
};

int main() {
    auto grid = generate_grid(9, 9);
    Variables<std::string> words{"MATTHEW", "JOE", "SARAH", "SALLY"};
    std::map<std::string, Domain> locations;
    Constraints<WordSearchConstraint> constraints;
    for (const auto& w: words) {
        locations[w] = generate_domain(w, grid);
        constraints.emplace_back(w);
    }
    CSP<WordSearchConstraint, Locations, std::string> csp{constraints, locations, words};
    for (const auto& [w, l] : backtracking_search(csp)) {
        assert(w.size() == l.size());
        std::cout << w << ':' << l << '\n';
        for (size_t i = 0, n = w.size(); i < n; ++i) {
            grid[l[i].row()][l[i].col()] = w[i];
        }
    }
    print_grid(grid);
}
