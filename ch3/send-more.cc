#include "common.h"
#include "csp.h"
#include "prettyprint.hpp"
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

std::vector<char> flatten(const std::vector<std::string>& v) {
    std::vector<char> flat;
    auto it = flat.begin();
    for (const auto& s: v) it = flat.insert(it, s.cbegin(), s.cend());
    return flat;
}

inline bool has_duplicate_values(const Assigment<char, int>& a) {
    return unique(map_values_to_vector(a)).size() < a.size();
}

std::pair<int, bool> word_to_value(const std::string& w, const Assigment<char, int>& a) {
    int n = 0;
    for (auto ch : w) {
        auto it = a.find(ch);
        if (it == a.end()) return {0, false};
        n = n * 10 + it->second;
    }
    return {n, true};
}

struct SendMoreMoneyConstraint {
    bool contains(char c) const { return c == ch; }
    bool is_satisfied(const Assigment<char, int>& a) const {
        if (has_duplicate_values(a)) return false;
        if (a.size() < n) return true;
        std::vector<int> values;
        for (const auto& w: words) {
            const auto [value, ok] = word_to_value(w, a);
            if (!ok) return false;
            values.push_back(value);
        }
        auto sum = std::accumulate(values.cbegin(), values.cend() - 1, 0);
        return sum == values.back();
    }
    char ch;
    size_t n;
    std::vector<std::string> words;
};

int main() {
    std::vector<std::string> words{"send", "more", "money"};
    Constraints<SendMoreMoneyConstraint> constraints;
    Domains<char, int> domains;
    auto variables = unique(flatten(words));
    for (auto ch: variables) {
        constraints.push_back({ch, variables.size(), words});
        domains.insert({ch, {0,1,2,3,4,5,6,7,8,9}});
    }
    std::cout << backtracking_search(constraints, domains, variables) << '\n';
}
