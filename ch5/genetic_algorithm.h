#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include "prettyprint.hpp"
#include "range/v3/all.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

template<typename C>
using Population = std::vector<C>;

template<typename C>
using Fitness = std::function<double(const C&)>;

template<typename C>
using RandomInstance = std::function<C()>;

template<typename C>
using Crossover = std::function<std::pair<C, C>(const C&, const C&)>;

template<typename C>
using Mutation = std::function<void(C&)>;

template<typename C>
using Picker = std::function<const C&(const Population<C>&, const Fitness<C>&)>;

// pick based on the proportion of summed total fitness that each
// individual represents
template<typename C>
struct Roulette
{
    const C& operator()(const Population<C>& population, const Fitness<C>& fitness_)
    {
        static std::random_device rd;
        static std::mt19937_64 gen{rd()};
        static std::uniform_real_distribution<double> dist{0, 1};

        using namespace ranges::v3;

        const auto fitness_cache = population
            | view::transform([&fitness_](const C& c){ return fitness_(c); });
        const auto sum = accumulate(fitness_cache, .0);
        const auto cumulative_dist = fitness_cache
            | view::transform([sum](const auto& f){ return f/sum; })
            | view::partial_sum();
        const auto p = dist(gen);
        const auto it = find_if(cumulative_dist, [p](double pc){ return p < pc; });
        const auto index = std::distance(cumulative_dist.begin(), it);
        return population[index];
    }
};

// find k random individuals in the population and pick the best one
template<typename C>
struct Tournament
{
    Tournament(size_t n = 4): n_{n} {}

    const C& operator()(const Population<C>& population, const Fitness<C>& fitness_)
    {
        std::vector<size_t> indexes(population.size());
        std::iota(indexes.begin(), indexes.end(), 0);
        std::random_shuffle(indexes.begin(), indexes.end());
        auto best = &population[indexes[0]];
        auto best_fitness = fitness_(*best);
        for (int i = 1; i < n_; i++) {
            auto candidate = &population[indexes[i]];
            auto fitness = fitness_(*candidate);
            if (fitness > best_fitness) {
                best = candidate;
                best_fitness = fitness;
            }
        }
        return *best;
    }

private:
    size_t n_;
};

template<typename C>
class GeneticAlgorithm
{
public:
    GeneticAlgorithm(double threshold, size_t size=10): threshold_(threshold), size_(size)
    {
        if (size_ == 0) throw std::runtime_error{""};
    }

    C run();

    Fitness<C> fitness_;
    RandomInstance<C> random_instance_;
    Crossover<C> crossover_;
    Mutation<C> mutation_;

private:
    void reproduce_and_replace();
    void mutate();
    auto get_parents();

    static inline std::random_device rd_;
    static inline std::mt19937_64 gen_{rd_()};
    static inline std::uniform_real_distribution<double> dist_{0, 1};

    double threshold_;
    size_t size_;
    Population<C> population_;
    unsigned max_generations_{100};
    double mutation_chance_{.01};
    double crossover_chance_{.7};
    Picker<C> pick_{Tournament<C>{}};
};

template<typename C>
C GeneticAlgorithm<C>::run()
{
    std::vector<double> fitness_cache(size_);
    population_.reserve(size_);
    std::generate_n(std::back_inserter(population_), size_, random_instance_);
    auto best = &population_[0];
    auto best_fitness = fitness_(*best);
    for (auto generation = 0; generation < max_generations_; ++generation) {
        std::cout
            << "generation " << generation << ", "
            << "best " << best_fitness << ", "
            << "avg " << "FIXME" << '\n';
        for (auto j = 0; j < size_; ++j) {
            fitness_cache[j] = fitness_(population_[j]);
            if (fitness_cache[j] > threshold_) {
                return population_[j];
            }
            if (fitness_cache[j] > best_fitness) {
                best_fitness = fitness_cache[j];
                best = &population_[j];
            }
        }
        reproduce_and_replace();
        mutate();
    }
    return *best;
}

template<typename C>
void GeneticAlgorithm<C>::reproduce_and_replace()
{
    Population<C> new_population;
    new_population.reserve(size_ + 1);
    while (new_population.size() < population_.size()) {
        const auto& [parent1, parent2] = get_parents();
        if (dist_(gen_) < crossover_chance_) {
            const auto [child1, child2] = crossover_(parent1, parent2);
            new_population.push_back(child1);
            new_population.push_back(child2);
        } else {
            new_population.push_back(parent1);
            new_population.push_back(parent2);
        }
    }
    if (new_population.size() > size_) {
        new_population.pop_back();
    }
    population_.swap(new_population);
}

template<typename C>
void GeneticAlgorithm<C>::mutate()
{
    for (auto& c: population_) {
        if (dist_(gen_) < mutation_chance_) {
            mutation_(c);
        }
    }
}

template<typename C>
auto GeneticAlgorithm<C>::get_parents()
{
    return std::make_pair(pick_(population_, fitness_), pick_(population_, fitness_));
}

#endif
