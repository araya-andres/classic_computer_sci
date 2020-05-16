#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

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

template<typename Chromosome>
using Population = std::vector<Chromosome>;

// Callbacks signatures.
template<typename Chromosome>
using FitnessFn = std::function<double(const Chromosome&)>;

template<typename Chromosome>
using RandomInstanceFn = std::function<Chromosome()>;

template<typename Chromosome>
using CrossoverFn = std::function<std::pair<Chromosome, Chromosome>(const Chromosome&, const Chromosome&)>;

template<typename Chromosome>
using MutateFn = std::function<void(Chromosome&)>;

template<typename Chromosome>
using PickFn = std::function<Chromosome(const Population<Chromosome>&, const FitnessFn<Chromosome>&)>;

// Default callbacks throw an exception if called.
template<typename Chromosome>
struct DefaultFitnessFn
{
    double operator()(const Chromosome&)
    {
        throw std::runtime_error{"no suitable fitness callback"};
    }
};

template<typename Chromosome>
struct DefaultRandomInstanceFn
{
    Chromosome operator()()
    {
        throw std::runtime_error{"no suitable random instance callback"};
    }
};

template<typename Chromosome>
struct DefaultCrossoverFn
{
    std::pair<Chromosome, Chromosome> operator()(const Chromosome&, const Chromosome&)
    {
        throw std::runtime_error{"no suitable crossover callback"};
    }
};

template<typename Chromosome>
struct DefaultMutateFn
{
    void operator()(Chromosome&)
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
template<typename Chromosome>
struct Roulette
{
    Chromosome operator()(const Population<Chromosome>& population, const FitnessFn<Chromosome>& fitness_fn)
    {
        using namespace ranges::v3;

        const auto fitness_cache = population
            | view::transform([&fitness_fn](const Chromosome& c){ return fitness_fn(c); });
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
template<typename Chromosome>
struct Tournament
{
    Tournament(size_t n = 5): n_{n} {}

    Chromosome operator()(const Population<Chromosome>& population, const FitnessFn<Chromosome>& fitness_fn)
    {
        std::random_device rd;
        std::mt19937_64 gen{rd()};
        std::vector<size_t> indexes(population.size());
        std::iota(indexes.begin(), indexes.end(), 0);
        std::shuffle(indexes.begin(), indexes.end(), gen);
        auto best = &population[indexes[0]];
        auto best_fitness = fitness_fn(*best);
        for (auto i = 1; i < n_; i++) {
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

template<typename Chromosome>
class GeneticAlgorithm
{
public:
    GeneticAlgorithm(double threshold, size_t size=100)
        : threshold_(threshold)
        , size_(size)
    {
        if (size_ == 0) {
            throw std::runtime_error{"population must be greater than 0"};
        }
    }

    std::pair<Chromosome, double> run();

    FitnessFn<Chromosome> fitness_fn{DefaultFitnessFn<Chromosome>{}};
    RandomInstanceFn<Chromosome> random_instance_fn{DefaultRandomInstanceFn<Chromosome>{}};
    CrossoverFn<Chromosome> crossover_fn{DefaultCrossoverFn<Chromosome>{}};
    MutateFn<Chromosome> mutate_fn{DefaultMutateFn<Chromosome>{}};
    PickFn<Chromosome> pick_fn{Tournament<Chromosome>{}};

private:
    void reproduce_and_replace();
    void mutate();
    auto get_parents();
    void generate_population();

    double threshold_;
    size_t size_;
    Population<Chromosome> population_;
    size_t max_generations_{100};
    double mutation_chance_{.01};
    double crossover_chance_{.5};
};

template<typename Chromosome>
std::pair<Chromosome, double> GeneticAlgorithm<Chromosome>::run()
{
    generate_population();
    auto best = &population_[0];
    auto best_fitness = fitness_fn(*best);
    for (size_t generation = 0; generation < max_generations_; ++generation) {
        auto fitness_sum = .0;
        for (size_t j = 0; j < size_; ++j) {
            auto fitness = fitness_fn(population_[j]);
            if (fitness > threshold_) {
                return {population_[j], fitness};
            }
            fitness_sum += fitness;
            if (fitness > best_fitness) {
                best_fitness = fitness;
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
    return {*best, best_fitness};
}

template<typename Chromosome>
void GeneticAlgorithm<Chromosome>::reproduce_and_replace()
{
    Population<Chromosome> new_population;
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
    while (new_population.size() > size_) {
        new_population.pop_back();
    }
    population_.swap(new_population);
}

template<typename Chromosome>
void GeneticAlgorithm<Chromosome>::mutate()
{
    for (auto& c: population_) {
        if (Random::get() < mutation_chance_) {
            mutate_fn(c);
        }
    }
}

template<typename Chromosome>
auto GeneticAlgorithm<Chromosome>::get_parents()
{
    auto parent1 = pick_fn(population_, fitness_fn);
    auto parent2 = pick_fn(population_, fitness_fn);
    return std::make_pair(parent1, parent2);
}

template<typename Chromosome>
void GeneticAlgorithm<Chromosome>::generate_population()
{
    population_.reserve(size_);
    std::generate_n(std::back_inserter(population_), size_, random_instance_fn);
}

#endif
