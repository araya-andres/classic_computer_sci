#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../stats.h"

TEST_CASE("Basic statistics", "[stats]") {
    const auto data = std::vector<double>{600, 470, 170, 430, 300};
    REQUIRE(stats::sum(data) == 1970);
    REQUIRE(stats::mean(data) == 394);
    REQUIRE(stats::variance(data) == 21704);
    REQUIRE(stats::stddev(data) == Approx(147.32277));
    REQUIRE(stats::zscored(data)[0] == Approx(1.39829));
}
