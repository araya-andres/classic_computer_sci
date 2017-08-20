#include "prettyprint.hpp"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <vector>

enum class Cell: char {
    Empty = ' ',
    Blocked = '#',
    Start = 'S',
    Goal = 'G',
    Path = 'P',
};

std::ostream& operator<<(std::ostream& os, Cell c) {
    return os << static_cast<char>(c);
}

using Maze = std::vector<std::vector<Cell>>;

struct Location { int row, col; };

bool operator==(const Location& rhs, const Location& lhs) {
    return rhs.row == lhs.row && rhs.col == lhs.col;
}

bool operator<(const Location& lhs, const Location& rhs) {
    return lhs.row < rhs.row || (lhs.row == rhs.row && lhs.col < rhs.col);
}

std::ostream& operator<<(std::ostream& os, const Location& l) {
    return os << '{' << l.row << ',' << l.col << '}';
}

bool is_within_maze(const Maze& m, const Location& l) {
    return (0 <= l.row && l.row < m.size())
        && (0 <= l.col && l.col < m[l.row].size());
}

bool is_cell_blocked(const Maze& m, const Location& l) {
    assert(is_within_maze(m, l));
    return m[l.row][l.col] == Cell::Blocked;
}

Maze generate_maze(int rows, int cols, double sparseness) {
    Maze m(rows);
    for (auto& r : m) {
        r.assign(cols, Cell::Empty);
        for (auto& c : r) {
            if (1.0 * rand() / RAND_MAX < sparseness) c = Cell::Blocked;
        }
    }
    return m;
}

void set_start_location(Maze& m, const Location& s) {
    assert(is_within_maze(m, s));
    m[s.row][s.col] = Cell::Start;
}

void print_maze(const Maze& m) {
    for (const auto& r : m) {
        for (const auto& c : r) {
            std::cout << c;
        }
        std::cout << '\n';
    }
}

using Successors = std::vector<Location>;

Successors successors_for_maze(const Maze& m, const Location& l) {
    Successors s {
        {l.row, l.col - 1},
        {l.row, l.col + 1},
        {l.row - 1, l.col},
        {l.row + 1, l.col},
    };
    s.erase(std::remove_if(s.begin(), s.end(), [&m](const Location& next) {
                return !is_within_maze(m, next) || is_cell_blocked(m, next);
                }), s.end());
    return s;
}

void dfs(const Maze& m, const Location& start, const Location& goal) {
    auto frontier = successors_for_maze(m, start);
    std::set<Location> explored{start};
    while (!frontier.empty()) {
        auto l = frontier.back();
        if (l == goal) {
            return;
        }
        frontier.pop_back();
        explored.insert(l);
        for (const auto& s : successors_for_maze(m, l)) {
            if (explored.find(s) == explored.end()) frontier.push_back(s);
        }
    }
}

int main() {
    srand(time(nullptr));
    const Location start_location{5,5};
    auto m = generate_maze(10, 10, 0.2);
    set_start_location(m, start_location);
    dfs(m,{5,5},{0,0});
    print_maze(m);
    return 0;
}
