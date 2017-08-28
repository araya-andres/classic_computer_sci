#include "bfs.h"
#include "prettyprint.hpp"
#include "range/v3/all.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <vector>

enum class Cell: char {
    Empty = ' ',
    Blocked = '#',
    Start = 'S',
    Goal = 'G',
    Path = '.',
};

std::ostream& operator<<(std::ostream& os, Cell c) {
    return os << static_cast<char>(c);
}

using Maze = std::vector<std::vector<Cell>>;

struct Location { int row, col; };

Location pick_random_location(const Maze& m) {
    return { int(rand() % m.size()), int(rand() % m[0].size()) };
}

bool operator==(const Location& lhs, const Location& rhs) {
    return lhs.row == rhs.row && lhs.col == rhs.col;
}

bool operator<(const Location& lhs, const Location& rhs) {
    return lhs.row < rhs.row || (lhs.row == rhs.row && lhs.col < rhs.col);
}

std::ostream& operator<<(std::ostream& os, const Location& l) {
    return os << '{' << l.row << ',' << l.col << '}';
}

bool is_within_maze(const Maze& m, const Location& l) {
    return (0 <= l.row && static_cast<size_t>(l.row) < m.size())
        && (0 <= l.col && static_cast<size_t>(l.col) < m[l.row].size());
}

bool is_cell_blocked(const Maze& m, const Location& l) {
    assert(is_within_maze(m, l));
    return m[l.row][l.col] == Cell::Blocked;
}

Maze generate_maze(int rows, int cols, double sparseness) {
    Maze m(rows);
    for (auto& row : m) {
        std::generate_n(std::back_inserter(row), cols, [&sparseness](){
                return (1.0 * rand() / RAND_MAX < sparseness)
                    ? Cell::Blocked
                    : Cell::Empty;
                });
    }
    return m;
}

void mark_start_location(Maze& m, const Location& s) {
    assert(is_within_maze(m, s));
    m[s.row][s.col] = Cell::Start;
}

void mark_goal_location(Maze& m, const Location& g) {
    assert(is_within_maze(m, g));
    m[g.row][g.col] = Cell::Goal;
}

std::ostream& operator<<(std::ostream& os, const Maze& m) {
    for (const auto& row : m) {
        for (const auto& cell : row) os << cell;
        os << '\n';
    }
    return os;
}

void mark_path(Maze& m, std::vector<Location>& p) {
    for (const auto& l : p) {
        m[l.row][l.col] = Cell::Path;
    }
}

struct GoalTest {
    const Location& goal;
    bool operator()(const Location& current_location) {
        return current_location == goal;
    }
};

struct Successors {
    const Maze& m;
    std::vector<Location> operator()(const Location& l) {
        using namespace ranges::view;
        std::vector<Location> candidates {
            {l.row - 1, l.col},
            {l.row, l.col + 1},
            {l.row + 1, l.col},
            {l.row, l.col - 1},
        };
        return candidates | remove_if([this](const Location& c) {
                return !is_within_maze(this->m, c) || is_cell_blocked(this->m, c);
                });
    }
};

int main(int argc, char* argv[]) {
    unsigned seed = time(nullptr);
    int size = 10;
    double sparseness = 0.2;
    int c;
    while ((c = getopt(argc, argv, "abds:S:")) != -1) {
        switch (c) {
            case 's': size = atoi(optarg); break;
            case 'S': seed = atoi(optarg); break;
        }
    }
    srand(seed);
    auto maze = generate_maze(size, size, sparseness);
    Location start_location, goal_location{size - 1, size - 1};
    auto path = bfs<Location>(start_location, GoalTest{goal_location}, Successors{maze});
    mark_path(maze, path);
    mark_start_location(maze, start_location);
    mark_goal_location(maze, goal_location);
    std::cout
        << "seed = " << seed << '\n'
        << maze << '\n';
}
