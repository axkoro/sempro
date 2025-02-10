#pragma once

#include "Graph.hpp"
#include "WeightedGraph.hpp"

class EdgeWeightCalculator {
   private:
    Graph& graph;
    double fusion_coefficient;

   public:
    EdgeWeightCalculator(Graph& g, double fusion_coefficient);

    WeightedGraph generate_weighted_graph();

   private:
    double compute_weight(int u, int v);
    double compute_feature_similarity(int u, int v);
    double compute_structural_similarity(int u, int v);
};