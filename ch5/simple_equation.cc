#include "genetic_algorithm.h"
#include "simple_equation.h"
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

void do_mutation(Point& p)
{
    if (ga_random() < .5) {
        p.x += (ga_random() < .5 ? 1 : -1);
    } else {
        p.y += (ga_random() < .5 ? 1 : -1);
    }
}

void solve_simple_equation()
{
    GeneticAlgorithm<Point> ga{13.0};
    ga.fitness_fn = [](const Point& p) -> double {
        return 6 * p.x - p.x * p.x + 4 * p.y - p.y * p.y;
    };
    ga.random_instance_fn = []() -> Point {
        return {MAX_VAL * ga_random() , MAX_VAL * ga_random()};
    };
    ga.crossover_fn = [](const Point& p, const Point& q) {
        return std::pair<Point, Point>{{p.x, q.y}, {q.x, p.y}};
    };
    ga.mutation_fn = do_mutation;
    std::cout << ga.run() << '\n';
}
