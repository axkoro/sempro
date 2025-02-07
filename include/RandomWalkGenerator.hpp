#pragma once

#include "StaticMinimalGraph.hpp"
#include <random>

class RandomWalkGenerator {
   private:
    const StaticMinimalGraph& graph;
    int walk_length;
    int num_walks;
    std::mt19937 rng; // Random number generator

   public:
    RandomWalkGenerator(const StaticMinimalGraph& graph, int walk_length, int num_walks, int seed = -1);
    void set_seed(int seed);
    std::vector<std::vector<int>> generate_walks();

   private:
    std::vector<int> perform_walk(int start_node);
};