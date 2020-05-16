#include <algorithm>
#include <cassert>
#include <map>
#include <utility>
#include <vector>

#include "prettyprint.hpp"

#include "genetic-algorithm.h"

using Chromosome = std::vector<char>;
using Index = std::map<char, size_t>;

const Chromosome letters{'S','E','N','D','M','O','R','E','Y',' ',' '};

int pow10(int exp) {
    auto ans = 1;
    for (; exp > 0; exp--) ans *= 10;
    return ans;
}

std::pair<Index, bool> get_index(const Chromosome& c)
{
    Index index;
    auto begin = c.cbegin();
    auto end = c.cend();
    for (auto i = 0; i < letters.size() - 2; ++i) {
        auto c = letters[i];
        auto it = std::find(begin, end, c);
        if (it == end) {
            return {{}, false};
        }
        index[c] = std::distance(it, begin);
    }
    return {index, true};
}

int word_to_value(const std::string word, const Index& index)
{
    auto value = 0;
    for (auto i = 0; i < word.size(); i++) {
        value += pow10(i) * index.at(word[i]);
    }
    return value;
}

double fitness(const Chromosome& c)
{
    if (auto [index, ok] = get_index(c); ok) {
        auto send = word_to_value("SEND", index);
        auto more = word_to_value("MORE", index);
        auto money = word_to_value("MONEY", index);
        auto diff = std::abs(money - send - more);
        return 1.0 / (diff + 1.0);
    }
    return .0;
}

Chromosome random_instance()
{
    Chromosome c{letters};
    std::random_device rd;
    std::mt19937_64 gen{rd()};
    std::shuffle(c.begin(), c.end(), gen);
    return c;
}

auto crossover(const Chromosome& parent1, const Chromosome& parent2)
{
    auto child1 = parent1;
    auto child2 = parent2;
    auto crossing_point = parent1.size() / 2;
    std::copy(parent2.begin() + crossing_point, parent2.end(), child1.begin() + crossing_point);
    std::copy(parent1.begin() + crossing_point, parent1.end(), child2.begin() + crossing_point);
    return std::make_pair(child1, child2);
}

void mutate(Chromosome& c)
{
    auto size = c.size();
    assert(size > 0);
    auto i = size * Random::get();
    auto j = size * Random::get();
    std::swap(c[i], c[j]);
}

int main()
{
    auto ga = GeneticAlgorithm<Chromosome>{1.0};
    ga.fitness_fn = fitness;
    ga.random_instance_fn = random_instance;
    ga.crossover_fn = crossover;
    ga.mutate_fn = mutate;
    auto best = ga.run();
    std::cout << best << '\n';
}
