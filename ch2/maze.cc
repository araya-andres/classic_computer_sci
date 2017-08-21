#include "prettyprint.hpp"
#include "range/v3/all.hpp"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
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
        row.assign(cols, Cell::Empty);
        for (auto& cell : row) {
            if (1.0 * rand() / RAND_MAX < sparseness) cell = Cell::Blocked;
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
    for (const auto& row : m) {
        for (const auto& cell : row) std::cout << cell;
        std::cout << '\n';
    }
}

using Successors = std::vector<Location>;

Successors successors_for_maze(const Maze& m, const Location& l) {
    using namespace ranges::view;
    Successors candidates {
        {l.row - 1, l.col},
        {l.row, l.col + 1},
        {l.row + 1, l.col},
        {l.row, l.col - 1},
    };
    return candidates | remove_if([&m](const Location& c) {
            return !is_within_maze(m, c) || is_cell_blocked(m, c);
            });
}

struct Node {
    Location l;
    const Node * prev;
    int cost;
};

bool operator<(const Node& lhs, const Node& rhs) {
    return lhs.l < rhs.l;
}

bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.l == rhs.l;
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
    return os << '{' << n.l << ',' << n.prev << ',' << n.cost << '}';
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
    std::stack<Node> frontier{std::deque<Node>{{start}}};
    std::set<Node> explored;
    auto it = explored.end();
    do {
        const auto current_node = frontier.top();
        if (current_node.l == goal) {
            return build_path(current_node.prev);
        }
        frontier.pop();
        std::tie(it, std::ignore) = explored.insert(current_node);
        for (const auto& s : successors_for_maze(m, current_node.l)) {
            Node n{s, &*it};
            if (explored.find(n) == explored.end()) frontier.push(n);
        }
    } while (!frontier.empty());
    return {};
}

Path bfs(const Maze& m, const Location& start, const Location& goal) {
    std::deque<Node> frontier{{start}};
    std::set<Node> explored;
    auto it = explored.end();
    do {
        const auto current_node = frontier.front();
        if (current_node.l == goal) {
            return build_path(current_node.prev);
        }
        frontier.pop_front();
        std::tie(it, std::ignore) = explored.insert(current_node);
        for (const auto& s : successors_for_maze(m, current_node.l)) {
            Node n{s, &*it};
            if (explored.find(n) == explored.end()) frontier.push_back(n);
        }
    } while (!frontier.empty());
    return {};
}

Path a_star(const Maze& m, const Location& start, const Location& goal) {
    auto cmp = [](const Node& lhs, const Node& rhs) { return lhs.cost > rhs.cost; };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> frontier{cmp, {{start}}};
    std::map<Node, int> explored;
    auto it = explored.end();
    bool success;
    do {
        const auto current_node = frontier.top();
        if (current_node.l == goal) {
            return build_path(current_node.prev);
        }
        frontier.pop();
        const auto cost = current_node.cost;
        std::tie(it, success) = explored.insert({current_node, cost});
        if (!success) { // node was already explored, update cost
            explored[current_node] = cost;
        }
        const auto prev = &it->first;
        const auto new_cost = cost + 1;
        for (const auto& s : successors_for_maze(m, current_node.l)) {
            Node n{s, prev, new_cost};
            it = explored.find(n);
            if (it == explored.end() || new_cost < it->second) frontier.push(n);
        }
    } while (!frontier.empty());
    return {};
}

Maze draw_path(Maze m, Path p) {
    for (const auto& l : p) {
        m[l.row][l.col] = Cell::Path;
    }
    return m;
}

void sandbox(const Maze& m, const Location& start, const Location& goal) {
}

int main() {
    srand(time(nullptr));
    const Location start_location{5, 5};
    const Location goal_location{0, 0};
    auto m = generate_maze(10, 10, 0.2);
    set_start_location(m, start_location);
    set_goal_location(m, goal_location);
    auto p = a_star(m, start_location, goal_location);
    print_maze(draw_path(m, p));
}
