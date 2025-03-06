#pragma once

#include <omp.h>

#include <iostream>
#include <queue>
#include <type_traits>
#include <unordered_set>

#include "EdgeWeightCalculator.hpp"  // because IntelliSense is stupid

template <typename T>
EdgeWeightCalculator<T>::EdgeWeightCalculator(AttributedGraph<T>& graph, double fusion_coefficient)
    : graph(graph), fusion_coefficient(fusion_coefficient) {}

template <typename T>
GraphEdgeWeights EdgeWeightCalculator<T>::generate_edge_weights() {
    GraphEdgeWeights edge_weights(graph);
    std::vector<std::unordered_set<int>> covers = compute_covers();

    for (auto it = edge_weights.begin(); it != edge_weights.end(); ++it) {
        auto [source, target] = it.get_edge();
        *it = compute_weight(source, target, covers);
    }
    return edge_weights;
}

template <typename T>
double EdgeWeightCalculator<T>::compute_weight(int u, int v,
                                               std::vector<std::unordered_set<int>>& covers) {
    double struct_sim = compute_structural_similarity(u, v, covers);
    double feature_sim = compute_feature_similarity(u, v);
    double weight = struct_sim * fusion_coefficient + feature_sim * (1 - fusion_coefficient);

    return weight;
}

template <typename T>
double EdgeWeightCalculator<T>::compute_feature_similarity(int u, int v) {
    int num_features = graph.get_num_features();
    int num_similar_features = 0;
#pragma omp parallel for reduction(+ : num_similar_features)
    for (int feature_idx = 0; feature_idx < num_features; feature_idx++) {
        if (have_similar_feature(u, v, feature_idx)) num_similar_features++;
    }

    double mfs = static_cast<double>(num_similar_features) / static_cast<double>(num_features);

    return mfs;
}

template <typename T>
bool EdgeWeightCalculator<T>::have_similar_feature(int node1, int node2, int feature_idx) {
    if (graph.is_missing(node1, feature_idx) || graph.is_missing(node2, feature_idx)) return false;
    T feature1 = graph.get_feature(node1, feature_idx);
    T feature2 = graph.get_feature(node2, feature_idx);
    if (std::is_floating_point_v<T>) {
        const double tolerance = 0.0001;
        return std::abs(feature1 - feature2) < tolerance;
    } else {
        return feature1 == feature2;
    }
}

template <typename T>
double EdgeWeightCalculator<T>::compute_structural_similarity(
    int u, int v, std::vector<std::unordered_set<int>>& covers) {
    const std::unordered_set<int>& cover_u = covers[u];
    const std::unordered_set<int>& cover_v = covers[v];

    std::vector<int> cover_u_vec(cover_u.begin(), cover_u.end());

    int intersection_size = 0;

#pragma omp parallel for reduction(+ : intersection_size)
    for (int i = 0; i < cover_u_vec.size(); ++i) {
        if (cover_v.count(cover_u_vec[i])) {
            intersection_size++;
        }
    }

    int union_size = cover_u.size() + cover_v.size() - intersection_size;

    double mss = static_cast<double>(intersection_size) / static_cast<double>(union_size);

    return mss;
}

template <typename T>
std::vector<std::unordered_set<int>> EdgeWeightCalculator<T>::compute_covers(int depth) {
    int num_nodes = graph.get_num_nodes();
    std::vector<std::unordered_set<int>> covers(num_nodes);

#pragma omp parallel for
    for (int node = 0; node < num_nodes; ++node) {
        auto neighbors = graph.get_k_hop_neighbors(node, depth);
        for (int neighbor : neighbors) {
            covers[node].insert(neighbor);
        }
        covers[node].erase(node);
    }

    return covers;
}