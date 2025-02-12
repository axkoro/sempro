#pragma once

#include <random>

#include "WeightedGraph.hpp"

class RandomWalkGenerator {
   private:
    int walk_length;
    int num_walks;

    const WeightedGraph& graph;
    std::mt19937 rng;  // Random number generator

   public:
    RandomWalkGenerator(const WeightedGraph& graph, int walk_length, int num_walks, int seed = -1);

    std::vector<std::vector<int>> generate_walks();

   private:
    std::vector<int> perform_walk(int start_node);
    int select_weighted_random_neighbor(const std::vector<WeightedGraph::Edge>& neighbors,
                                        std::mt19937& rng);
};