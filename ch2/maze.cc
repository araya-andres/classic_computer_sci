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
        row.assign(cols, Cell::Empty);
        for (auto& cell : row) {
            if (1.0 * rand() / RAND_MAX < sparseness) cell = Cell::Blocked;
        }
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
    Location location;
    const Node * parent;
    int cost;
};

bool operator<(const Node& lhs, const Node& rhs) {
    return lhs.location < rhs.location;
}

bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.location == rhs.location;
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
    return os << '{' << n.location << ',' << n.parent << ',' << n.cost << '}';
}

using Path = std::vector<Location>;

Path build_path(const Node* n) {
    assert(n);
    Path p;
    while (n) {
        p.push_back(n->location);
        n = n->parent;
    }
    return p;
}

Path dfs(const Maze& m, const Location& start, const Location& goal) {
    std::stack<Node> frontier{std::deque<Node>{{start}}};
    std::set<Node> explored;
    auto it = explored.end();
    do {
        const auto current_node = frontier.top();
        const auto current_location = current_node.location;
        if (current_location == goal) {
            return build_path(&current_node);
        }
        frontier.pop();
        std::tie(it, std::ignore) = explored.insert(current_node);
        for (const auto& s : successors_for_maze(m, current_location)) {
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
        const auto current_location = current_node.location;
        if (current_location == goal) {
            return build_path(&current_node);
        }
        std::tie(it, std::ignore) = explored.insert(current_node);
        for (const auto& s : successors_for_maze(m, current_location)) {
            Node n{s, &*it};
            if (explored.find(n) == explored.end()) frontier.push_back(n);
        }
        frontier.pop_front();
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
        const auto current_location = current_node.location;
        if (current_location == goal) {
            return build_path(&current_node);
        }
        const auto cost = current_node.cost;
        std::tie(it, success) = explored.insert({current_node, cost});
        if (!success) { // node was already explored, update cost
            explored[current_node] = cost;
        }
        const auto parent = &it->first;
        const auto new_cost = cost + 1;
        for (const auto& s : successors_for_maze(m, current_location)) {
            Node n{s, parent, new_cost};
            it = explored.find(n);
            if (it == explored.end() || new_cost < it->second) frontier.push(n);
        }
        frontier.pop();
    } while (!frontier.empty());
    return {};
}

void mark_path(Maze& m, Path p) {
    for (const auto& l : p) {
        m[l.row][l.col] = Cell::Path;
    }
}

using MazeSolver = Path(*)(const Maze&, const Location&, const Location&);

int main(int argc, char* argv[]) {
    MazeSolver solver = a_star;
    unsigned seed = time(nullptr);
    int size = 10;
    double sparseness = 0.2;
    int c;
    while ((c = getopt(argc, argv, "abds:S:")) != -1) {
        switch (c) {
            case 'a': solver = a_star; break;
            case 'b': solver = bfs; break;
            case 'd': solver = dfs; break;
            case 's': size = atoi(optarg); break;
            case 'S': seed = atoi(optarg); break;
        }
    }
    srand(seed);
    auto maze = generate_maze(size, size, sparseness);
    auto start_location = pick_random_location(maze);
    auto goal_location = pick_random_location(maze);
    auto path = solver(maze, start_location, goal_location);
    mark_path(maze, path);
    mark_start_location(maze, start_location);
    mark_goal_location(maze, goal_location);
    std::cout
        << "seed = " << seed << '\n'
        << maze << '\n';
}
