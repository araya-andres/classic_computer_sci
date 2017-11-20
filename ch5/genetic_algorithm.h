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

// Callbacks signatures.
template<typename C>
using FitnessFn = std::function<double(const C&)>;

template<typename C>
using RandomInstanceFn = std::function<C()>;

template<typename C>
using CrossoverFn = std::function<std::pair<C, C>(const C&, const C&)>;

template<typename C>
using MutateFn = std::function<void(C&)>;

template<typename C>
using PickFn = std::function<const C&(const Population<C>&, const FitnessFn<C>&)>;

// Default callbacks throw an exception if called.
template<typename C>
struct DefaultFitnessFn
{
    double operator()(const C&)
    {
        throw std::runtime_error{"no suitable fitness callback"};
    }
};

template<typename C>
struct DefaultRandomInstanceFn
{
    C operator()()
    {
        throw std::runtime_error{"no suitable random instance callback"};
    }
};

template<typename C>
struct DefaultCrossoverFn
{
    std::pair<C, C> operator()(const C&, const C&)
    {
        throw std::runtime_error{"no suitable crossover callback"};
    }
};

template<typename C>
struct DefaultMutateFn
{
    void operator()(C&)
    {
        // do nothing
    }
};

// Returns a random number between 0 (inclusive) and 1 (exclusive)
struct Random
{
    static double get()
    {
        static std::random_device rd;
        static std::mt19937_64 gen{rd()};
        static std::uniform_real_distribution<> dist{.0, 1.0};
        return dist(gen);
    }
};

// Pick based on the proportion of summed total fitness that each
// individual represents
template<typename C>
struct Roulette
{
    const C& operator()(const Population<C>& population, const FitnessFn<C>& fitness_fn)
    {
        using namespace ranges::v3;

        const auto fitness_cache = population
            | view::transform([&fitness_fn](const C& c){ return fitness_fn(c); });
        const auto sum = accumulate(fitness_cache, .0);
        const auto cumulative_dist = fitness_cache
            | view::transform([sum](const auto& f){ return f/sum; })
            | view::partial_sum();
        const auto p = Random::get();
        const auto it = find_if(cumulative_dist, [p](double pc){ return p < pc; });
        const auto index = std::distance(cumulative_dist.begin(), it);
        return population[index];
    }
};

// Find k random individuals in the population and pick the best one
template<typename C>
struct Tournament
{
    Tournament(size_t n = 4): n_{n} {}

    const C& operator()(const Population<C>& population, const FitnessFn<C>& fitness_fn)
    {
        std::vector<size_t> indexes(population.size());
        std::iota(indexes.begin(), indexes.end(), 0);
        std::random_shuffle(indexes.begin(), indexes.end());
        auto best = &population[indexes[0]];
        auto best_fitness = fitness_fn(*best);
        for (size_t i = 1; i < n_; i++) {
            auto candidate = &population[indexes[i]];
            auto candidate_fitness = fitness_fn(*candidate);
            if (candidate_fitness > best_fitness) {
                best = candidate;
                best_fitness = candidate_fitness;
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
    GeneticAlgorithm(double threshold, size_t size=10)
        : threshold_(threshold)
        , size_(size)
    {
        if (size_ == 0) {
            throw std::runtime_error{"population must be greater than 0"};
        }
    }

    C run();

    FitnessFn<C> fitness_fn{DefaultFitnessFn<C>{}};
    RandomInstanceFn<C> random_instance_fn{DefaultRandomInstanceFn<C>{}};
    CrossoverFn<C> crossover_fn{DefaultCrossoverFn<C>{}};
    MutateFn<C> mutate_fn{DefaultMutateFn<C>{}};
    PickFn<C> pick_fn{Tournament<C>{}};

private:
    void reproduce_and_replace();
    void mutate();
    auto get_parents();

    double threshold_;
    size_t size_;
    Population<C> population_;
    size_t max_generations_{100};
    double mutation_chance_{.01};
    double crossover_chance_{.7};
};

template<typename C>
C GeneticAlgorithm<C>::run()
{
    std::vector<double> fitness_cache(size_);
    population_.reserve(size_);
    std::generate_n(std::back_inserter(population_), size_, random_instance_fn);
    auto best = &population_[0];
    auto best_fitness = fitness_fn(*best);
    for (size_t generation = 0; generation < max_generations_; ++generation) {
        auto fitness_sum = .0;
        for (size_t j = 0; j < size_; ++j) {
            fitness_cache[j] = fitness_fn(population_[j]);
            if (fitness_cache[j] > threshold_) {
                return population_[j];
            }
            fitness_sum += fitness_cache[j];
            if (fitness_cache[j] > best_fitness) {
                best_fitness = fitness_cache[j];
                best = &population_[j];
            }
        }
        std::cout
            << "generation " << generation << ", "
            << "best " << best_fitness << ", "
            << "avg " << fitness_sum/size_ << '\n';
        reproduce_and_replace();
        mutate();
    }
    return *best;
}

template<typename C>
void GeneticAlgorithm<C>::reproduce_and_replace()
{
    Population<C> new_population;
    new_population.reserve(size_);
    while (new_population.size() < population_.size()) {
        const auto& [parent1, parent2] = get_parents();
        if (Random::get() < crossover_chance_) {
            const auto [child1, child2] = crossover_fn(parent1, parent2);
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
        if (Random::get() < mutation_chance_) {
            mutate_fn(c);
        }
    }
}

template<typename C>
auto GeneticAlgorithm<C>::get_parents()
{
    return std::make_pair(
            pick_fn(population_, fitness_fn),
            pick_fn(population_, fitness_fn)
            );
}

#endif
