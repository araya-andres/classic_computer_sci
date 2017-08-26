#ifndef BFS_H
#define BFS_H

#include <cassert>
#include <deque>
#include <iostream>
#include <queue>
#include <set>
#include <tuple>
#include <vector>

template <typename State>
struct Node {
    State state;
    const Node* parent;
};

template <typename State>
bool operator<(const Node<State>& lhs, const Node<State>& rhs) {
    return lhs.state < rhs.state;
}

template <typename State>
bool operator==(const Node<State>& lhs, const Node<State>& rhs) {
    return lhs.state == rhs.state;
}

template <typename State>
std::ostream& operator<<(std::ostream& os, const Node<State>& n) {
    return os << '{' << n.state << ',' << n.parent << '}';
}

template <typename State>
using Path = std::vector<State>;

template <typename State>
Path<State> build_path(const Node<State>* n) {
    assert(n);
    Path<State> tmp;
    while (n) {
        tmp.push_back(n->state);
        n = n->parent;
    }
    return {tmp.rbegin(), tmp.rend()};
}

template <typename State>
using GoalTestFn = std::function<bool(const State&)>;

template <typename State>
using SuccessorsFn = std::function<std::vector<State>(const State&)>;

template <typename State>
Path<State> bfs(
        const State& initial_state,
        GoalTestFn<State> goal_test,
        SuccessorsFn<State> successors)
{
    std::deque<Node<State>> frontier{{initial_state}};
    std::set<Node<State>> explored;
    do {
        const auto& current_node = frontier.front();
        const auto& current_state = current_node.state;
        if (goal_test(current_state)) {
            return build_path(&current_node);
        }
        const auto [it, _] = explored.insert(current_node);
        for (const auto& s : successors(current_state)) {
            Node<State> n{s, &*it};
            if (explored.find(n) == explored.end()) frontier.push_back(n);
        }
        frontier.pop_front();
    } while (!frontier.empty());
    return {};
}

#endif
