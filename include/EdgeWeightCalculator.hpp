#pragma once

#include "StaticMinimalGraph.hpp"

class EdgeWeightCalculator {
   private:
    double fusion_coefficient;

   public:
    EdgeWeightCalculator(const Graph& g, double fusion_coefficient);

    StaticMinimalGraph generate_weighted_graph();

   private:
    double compute_weight(int u, int v);
    double compute_feature_similarity(int u, int v);
    double compute_structural_similarity(int u, int v);
};