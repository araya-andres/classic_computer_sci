#include "prettyprint.hpp"
#include "genetic-algorithm.h"
#include <algorithm>
#include <vector>
#include <utility>

using Genes = std::vector<char>;

const Genes letters{'S','E','N','D','M','O','R','E','Y',' ',' '};

double fitness(const Genes& genes)
{
    return .0;
}

Genes random_instance()
{
    Genes genes{letters};
    std::random_shuffle(genes.begin(), genes.end());
    return genes;
}

auto crossover(const Genes& parent1, const Genes& parent2)
{
    auto child1 = parent1;
    auto child2 = parent2;
    auto crossing_point = parent1.size() / 2;
    std::copy(parent2.begin() + crossing_point, parent2.end(), child1.begin() + crossing_point);
    std::copy(parent1.begin() + crossing_point, parent1.end(), child2.begin() + crossing_point);
    return std::make_pair(child1, child2);
}

void mutate(Genes& genes)
{
    auto size = genes.size();
    int i = size * Random::get();
    int j = size * Random::get();
    if (Random::get() < .5) {
        genes[i] = letters[j];
    } else {
        std::swap(genes[i], genes[j]);
    }
}

int main()
{
    GeneticAlgorithm<Genes> ga{.0}; // FIXME
    ga.random_instance_fn = random_instance;
    ga.crossover_fn = crossover;
    ga.mutate_fn = mutate;
}
