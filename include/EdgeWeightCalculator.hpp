#pragma once

#include <unordered_set>

#include "AttributedGraph.hpp"
#include "Graph.hpp"
#include "GraphEdgeWeights.hpp"

#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test

template <typename T>
class EdgeWeightCalculator {
    FRIEND_TEST(EdgeWeightCalculatorTest, ComputeCovers);
    FRIEND_TEST(EdgeWeightCalculatorTest, cover_union_intersection);

   private:
    AttributedGraph<T>& graph;
    double fusion_coefficient;

    std::vector<std::unordered_set<int>> covers;

   public:
    EdgeWeightCalculator(AttributedGraph<T>& g, double fusion_coefficient);
    GraphEdgeWeights generate_edge_weights();

   private:
    double compute_weight(int u, int v, std::vector<std::unordered_set<int>>& covers);
    double compute_feature_similarity(int u, int v);
    bool have_similar_feature(int node1, int node2, int feature_idx);
    double compute_structural_similarity(int u, int v,
                                         std::vector<std::unordered_set<int>>& covers);
    std::vector<std::unordered_set<int>> compute_covers(int depth = 2);
};

#include "EdgeWeightCalculator.tpp"