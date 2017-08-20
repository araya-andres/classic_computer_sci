#include "prettyprint.hpp"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <vector>
#include <tuple>

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

void set_goal_location(Maze& m, const Location& g) {
    assert(is_within_maze(m, g));
    m[g.row][g.col] = Cell::Goal;
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

struct Node {
    Location l;
    const Node * prev;
};

bool operator<(const Node& lhs, const Node& rhs) {
    return lhs.l < rhs.l;
}

bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.l == rhs.l;
}

using Path = std::vector<Location>;

Path build_path(const Node* n) {
    Path p;
    while (n) {
        p.push_back(n->l);
        n = n->prev;
    }
    return p;
}

Path dfs(const Maze& m, const Location& start, const Location& goal) {
    std::set<Node> explored{{start, nullptr}};
    std::vector<Node> frontier;
    for (const auto& l : successors_for_maze(m, start)) frontier.push_back({l, nullptr});
    while (!frontier.empty()) {
        auto current_node = frontier.back();
        if (current_node.l == goal) {
            return build_path(current_node.prev);
        }
        frontier.pop_back();
        auto [it, _] = explored.insert(current_node);
        for (const auto& s : successors_for_maze(m, current_node.l)) {
            Node n{s, &*it};
            if (explored.find(n) == explored.end()) frontier.push_back(n);
        }
    }
    return {};
}

Maze draw_path(Maze m, Path p) {
    for (const auto& l : p) {
        m[l.row][l.col] = Cell::Path;
    }
    return m;
}

int main() {
    srand(time(nullptr));
    const Location start_location{5,5};
    const Location goal_location{0,0};
    auto m = generate_maze(10, 10, 0.2);
    set_start_location(m, start_location);
    set_goal_location(m, goal_location);
    auto p = dfs(m, start_location, goal_location);
    print_maze(draw_path(m, p));
}
