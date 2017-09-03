#include "csp.h"
#include "prettyprint.hpp"
#include <algorithm>
#include <iostream>

struct QueenConstraint {
    int col;
    bool contains(int c) const { return col == c; }
    bool is_satisfied(const Assigment<int, int>& a) const {
        const auto q1c = col;
        const auto it  = a.find(q1c);
        const auto q1r = it->second;
        for (const auto [q2c, q2r] : a) {
            if (q1c == q2c) continue;
            if (q1r == q2r) return false;
            if (abs(q1c - q2c) == abs(q1r - q2r)) return false;
        }
        return true;
    }
};

int main() {
    std::vector<int> values{1,2,3,4,5,6,7,8};
    Domains<int, int> domains;
    Constraints<QueenConstraint> constraints;
    for (auto v : values) {
        domains[v] = values;
        constraints.push_back({v});
    }
    std::cout << backtracking_search(constraints, domains, values) << '\n';
}
