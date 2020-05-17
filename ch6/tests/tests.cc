#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../point.h"
#include "../stats.h"

TEST_CASE("Basic statistics", "[stats]") {
    const auto data = std::vector<double>{600, 470, 170, 430, 300};
    REQUIRE(stats::sum(data) == 1970);
    REQUIRE(stats::mean(data) == 394);
    REQUIRE(stats::variance(data) == 21704);
    REQUIRE(stats::stddev(data) == Approx(147.32277));
    REQUIRE(stats::zscored(data)[0] == Approx(1.39829));
}

TEST_CASE("Point", "[point]") {
    REQUIRE(Point<2>{} == Point<2>{0, 0});
    REQUIRE(distance(Point<2>{}, Point<2>{1, 1}) == Approx(1.41421));
    REQUIRE(distance(Point<3>{2, 1, 1}, Point<3>{2, 2, 5}) == Approx(4.12311));
    SECTION("slice") {
        auto points = std::vector<Point<3>>{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        REQUIRE(dimension_slice(points, 0) == std::vector<double>{1, 0, 0});
        REQUIRE(dimension_slice(points, 1) == std::vector<double>{0, 1, 0});
        REQUIRE(dimension_slice(points, 2) == std::vector<double>{0, 0, 1});
    }
}
