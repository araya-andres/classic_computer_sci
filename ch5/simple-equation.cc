#include "genetic-algorithm.h"
#include "prettyprint.hpp"
#include <iostream>

const static double MAX_VAL = 100.0;

struct Point
{
    double x;
    double y;
};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
    return os << '{' << p.x << ',' << p.y << '}';
}

int main()
{
    GeneticAlgorithm<Point> ga{13.0};
    ga.fitness_fn = [](const Point& p) -> double {
        return 6 * p.x - p.x * p.x + 4 * p.y - p.y * p.y;
    };
    ga.random_instance_fn = []() -> Point {
        return {MAX_VAL * Random::get() , MAX_VAL * Random::get()};
    };
    ga.crossover_fn = [](const Point& p, const Point& q) -> std::pair<Point, Point> {
        return {{p.x, q.y}, {q.x, p.y}};
    };
    ga.mutate_fn = [](Point& p) {
        if (Random::get() < .5) {
            p.x += (Random::get() < .5 ? 1 : -1);
        } else {
            p.y += (Random::get() < .5 ? 1 : -1);
        }
    };
    std::cout << ga.run() << '\n';
}
