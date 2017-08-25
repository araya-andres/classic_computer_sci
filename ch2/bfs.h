#ifndef BFS_H
#define BFS_H

#include <cassert>
#include <deque>
#include <iostream>
#include <queue>
#include <set>
#include <tuple>
#include <vector>

template <typename S>
struct Node {
    S state;
    const Node* parent;
};

template <typename S>
bool operator<(const Node<S>& lhs, const Node<S>& rhs) {
    return lhs.state < rhs.state;
}

template <typename S>
bool operator==(const Node<S>& lhs, const Node<S>& rhs) {
    return lhs.state == rhs.state;
}

template <typename S>
std::ostream& operator<<(std::ostream& os, const Node<S>& n) {
    return os << '{' << n.state << ',' << n.parent << '}';
}

template <typename S>
std::vector<S> build_path(const Node<S>* n) {
    assert(n);
    std::vector<S> p;
    while (n) {
        p.push_back(n->state);
        n = n->parent;
    }
    return p;
}

template <typename S>
std::vector<S> bfs(
        const S& initial_state,
        std::function<bool(const S&)> goal_test,
        std::function<std::vector<S>(const S&)> successors)
{
    std::deque<Node<S>> frontier{{initial_state}};
    std::set<Node<S>> explored;
    do {
        const auto current_node = frontier.front();
        const auto current_state = current_node.state;
        if (goal_test(current_state)) {
            return build_path(&current_node);
        }
        const auto [it, _] = explored.insert(current_node);
        for (const auto& s : successors(current_state)) {
            Node<S> n{s, &*it};
            if (explored.find(n) == explored.end()) frontier.push_back(n);
        }
        frontier.pop_front();
    } while (!frontier.empty());
    return {};
}

#endif
