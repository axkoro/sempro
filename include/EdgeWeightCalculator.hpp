#pragma once

#include "Graph.hpp"
#include "WeightedGraph.hpp"
#include <unordered_set>
#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test
class EdgeWeightCalculator {
    FRIEND_TEST(EdgeWeightCalculatorTest, ComputeCovers);
    FRIEND_TEST(EdgeWeightCalculatorTest, cover_union_intersection);
   private:
    Graph& graph;
    double fusion_coefficient;
    std::vector<std::unordered_set<int>> covers;

   public:
    EdgeWeightCalculator(Graph& g, double fusion_coefficient);
    WeightedGraph generate_weighted_graph();

   private:
    double compute_weight(int u, int v,std::vector<std::unordered_set<int>> covers);
    double compute_feature_similarity(int u, int v);
    double compute_structural_similarity(int u, int v,std::vector<std::unordered_set<int>> covers);
    std::vector<std::unordered_set<int>> compute_covers(WeightedGraph& wgraph);
};