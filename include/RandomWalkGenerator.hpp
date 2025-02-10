#pragma once

#include <random>

#include "StaticMinimalGraph.hpp"

class RandomWalkGenerator {
   private:
    const StaticMinimalGraph& graph;
    int walk_length;
    int num_walks;
    std::mt19937 rng;  // Random number generator

   public:
    RandomWalkGenerator(const StaticMinimalGraph& graph, int walk_length, int num_walks,
                        int seed = -1);
    void set_seed(int seed);
    int select_weighted_random_neighbor(const std::vector<Edge>& neighbors, std::mt19937& rng);
    std::vector<std::vector<int>> generate_walks();

   private:
    std::vector<int> perform_walk(int start_node);
};