#ifndef STATS_H
#define STATS_H

#include <algorithm>
#include <cmath>
#include <numeric>

namespace stats {

template <typename C>
double sum(const C& c) {
    return std::accumulate(std::cbegin(c), std::cend(c), 0);
}

// Find the average (mean)
template <typename C>
double mean(const C& c) {
    return sum(c) / c.size();
}

// Find the variance: sum((Xi  - mean)^2) / N
template <typename C>
double variance(const C& c) {
    auto const m = mean(c);
    auto ans = 0.;
    for (const auto& val: c) {
        const auto diff = val - m;
        ans += (diff * diff);
    }
    return ans / c.size();
}

// Find the standard deviation
template <typename C>
double stddev(const C& c) {
    return std::sqrt(variance(c));
}

// Convert elements to respective z-score: (x - mean) / std
template <typename C>
C zscored(const C& c) {
    const auto m = mean(c);
    const auto s = stddev(c);
    auto ans = c;
    std::transform(
        ans.begin(), ans.end(), ans.begin(),
        [&](auto x){ return (x - m) / s; }
    );
    return ans;
}

}

#endif
