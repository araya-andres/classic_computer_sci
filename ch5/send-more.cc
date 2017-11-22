#include "prettyprint.hpp"
#include "genetic-algorithm.h"
#include <algorithm>
#include <vector>
#include <utility>
#include <map>

using Genes = std::vector<char>;
using Index = std::map<char, size_t>;

const Genes letters{'S','E','N','D','M','O','R','E','Y',' ',' '};

std::pair<Index, bool> get_index(const Genes& genes)
{
    Index index;
    auto begin = genes.cbegin();
    auto end = genes.cend();
    for (size_t i = 0, n = letters.size() - 2; i < n; ++i) {
        char c = letters[i];
        auto it = std::find(begin, end, c);
        if (it == end) {
            return {{}, false};
        }
        index[c] = std::distance(it, begin);
    }
    return {index, true};
}

int word_to_value(const std::string& word, const Index& index)
{
    int value = 0;
    for (auto c: word) {
        value = 10 * value + index.at(c);
    }
    return value;
}

double fitness(const Genes& genes)
{
    if (auto [index, ok] = get_index(genes); ok) {
        auto send = word_to_value("SEND", index);
        auto more = word_to_value("MORE", index);
        auto money = word_to_value("MONEY", index);
        auto diff = std::abs(money - send - more);
        return 1.0 / (diff + 1.0);
    }
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
    GeneticAlgorithm<Genes> ga{1.0};
    ga.fitness_fn = fitness;
    ga.random_instance_fn = random_instance;
    ga.crossover_fn = crossover;
    ga.mutate_fn = mutate;
    auto best = ga.run();
    std::cout << best << '\n';
}
