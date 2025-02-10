#pragma once

#include "StaticMinimalGraph.hpp"

class RandomWalkGenerator {
   private:
    int walk_length;
    int num_walks;

   public:
    RandomWalkGenerator(const StaticMinimalGraph& graph, int walk_length, int num_walks,
                        int seed = -1);

    std::vector<std::vector<int>>
    generate_walks();  // TODO: potential optimization: return using move semantics (benchmark this
                       // before changing!!)

   private:
    std::vector<int> perform_walk(int start_node);
};