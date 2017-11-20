#include "prettyprint.hpp"
#include <iostream>
#include <random>

#include "genetic_algorithm.h"

static std::random_device rd;
static std::mt19937_64 gen{rd()};
static std::uniform_real_distribution<double> dist{0, 1};

const static double MAX_VAL = 100.0;

struct Point {
    double x;
    double y;
};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
    return os << '{' << p.x << ',' << p.y << '}';
}

void do_mutation(Point& p)
{
    if (dist(gen) < .5) {
        p.x += (dist(gen) < .5 ? 1 : -1);
    } else {
        p.y += (dist(gen) < .5 ? 1 : -1);
    }
}

int main()
{
    gen.seed(time(nullptr));
    GeneticAlgorithm<Point> ga{13.0};
    ga.fitness_fn = [](const Point& p){ return 6*p.x - p.x*p.x + 4*p.y - p.y*p.y; };
    ga.random_instance_fn = []{ return Point{MAX_VAL * dist(gen) , MAX_VAL * dist(gen)}; };
    ga.crossover_fn = [](const Point& p, const Point& q){ return std::pair<Point, Point>{{p.x, q.y}, {q.x, p.y}}; };
    ga.mutation_fn = do_mutation;
    std::cout << ga.run() << '\n';
}
