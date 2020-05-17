#ifndef POINT_H
#define POINT_H

#include <array>
#include <cmath>

#include "prettyprint.hpp"

template <size_t N>
using Point = std::array<double, N>;

template <size_t N>
double distance(const Point<N>& from, const Point<N>& to) {
    auto temp = .0;
    for (auto i = 0; i < N; ++i) {
        const auto d = to[i] - from[i];
        temp += d * d;
    }
    return std::sqrt(temp);
}

template <size_t N>
std::vector<double> dimension_slice(const std::vector<Point<N>>& points, const size_t index) {
    auto ans = std::vector<double>(points.size());
    std::transform(
            std::begin(points), std::end(points),
            std::begin(ans),
            [&index](const auto& p){ return p[index]; }
            );
    return ans;
}

#endif
