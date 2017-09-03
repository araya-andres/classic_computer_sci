#ifndef CSP_H
#define CSP_H

#include <algorithm>
#include <cassert>
#include <iterator>
#include <map>
#include <vector>

template <typename C> using Constraints = std::vector<C>;
template <typename V, typename D> using Domains = std::map<V, std::vector<D>>;
template <typename V> using Variables = std::vector<V>;

template <typename V, typename D>
using Assigment = std::map<V, D>;

template <typename V, typename D>
struct BinaryConstraint {
    V variable1;
    V variable2;
    bool contains(const V& variable) const {
        return variable1 == variable || variable2 == variable;
    }
    bool is_satisfied(const Assigment<V, D>& a) const {
        const auto it1 = a.find(variable1);
        const auto it2 = a.find(variable2);
        const auto end = a.cend();
        return (it1 == end || it2 == end) || it1->second != it2->second;
    }
};

template <typename C, typename D, typename V>
bool is_consistent(const V& variable, const Assigment<V, D>& assigment, const Constraints<C>& constraints) {
    for (const auto& c : constraints) {
        if (!c.contains(variable)) continue;
        if (!c.is_satisfied(assigment)) return false;
    }
    return true;
}

template <typename C, typename D, typename V>
Assigment<V, D> backtracking_search(
        const Constraints<C>& constraints,
        const Domains<V, D>& domains,
        const Variables<V>& variables,
        const Assigment<V, D>& assigment) {
    if (assigment.size() == variables.size()) return assigment;
    const auto it = std::find_if(variables.cbegin(), variables.cend(),
            [&assigment](const V& v) { return assigment.find(v) == assigment.end(); });
    assert(it != variables.cend());
    const auto& variable = *it;
    for (const auto& value : domains.at(variable)) {
        auto local_assigment = assigment;
        local_assigment[variable] = value;
        if (is_consistent<C, D, V>(variable, local_assigment, constraints)) {
            const auto ans = backtracking_search(constraints, domains, variables, local_assigment);
            if (!ans.empty()) return ans;
        }
    }
    return {};
}

template <typename C, typename D, typename V>
Assigment<V, D> backtracking_search(
        const Constraints<C>& constraints,
        const Domains<V, D>& domains,
        const Variables<V>& variables) {
    return backtracking_search(constraints, domains, variables, {});
}

#endif
