#include "csp.h"
#include "prettyprint.hpp"
#include <algorithm>
#include <iostream>

struct QueenConstraint {
    int variable1;
    int variable2; // unused
    bool is_satisfied(const Assigment<int, int>& a) const {
        const auto q1c = variable1;
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
    auto values = std::vector<int>{1,2,3,4,5,6,7,8};
    Domains<int, int> domains;
    Constraints<QueenConstraint> constraints;
    for (auto v : values) {
        domains[v] = values;
        constraints.push_back({v});
    }
    CSP<QueenConstraint, int, int> csp{constraints, domains, values};
    std::cout << backtracking_search<QueenConstraint, int, int>(csp) << '\n';
}
