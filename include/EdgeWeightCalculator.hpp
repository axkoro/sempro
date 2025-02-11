#pragma once

#include "Graph.hpp"
#include "WeightedGraph.hpp"
#include <unordered_set>
class EdgeWeightCalculator {
   private:
    Graph& graph;
    double fusion_coefficient;
    std::vector<std::unordered_set<int>> covers;

   public:
    EdgeWeightCalculator(Graph& g, double fusion_coefficient);
    std::vector<std::unordered_set<int>> compute_covers(WeightedGraph& wgraph);
    WeightedGraph generate_weighted_graph();

   private:
    double compute_weight(int u, int v,std::vector<std::unordered_set<int>> covers);
    double compute_feature_similarity(int u, int v);
    double compute_structural_similarity(int u, int v,std::vector<std::unordered_set<int>> covers);
    
};