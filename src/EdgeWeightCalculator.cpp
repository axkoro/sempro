#include "EdgeWeightCalculator.hpp"

#include <iostream>
#include <queue>
#include <unordered_set>

#include "WeightedGraph.hpp"

EdgeWeightCalculator::EdgeWeightCalculator(Graph& graph, double fusion_coefficient)
    : graph(graph), fusion_coefficient(fusion_coefficient), type(graph.get_type()) {}

WeightedGraph EdgeWeightCalculator::generate_weighted_graph() {
    WeightedGraph wgraph(graph);
    std::vector<std::unordered_set<int>> covers = compute_covers();

    for (auto it = wgraph.begin(); it != wgraph.end(); ++it) {
        auto [source, target] = it.get_edge();
        *it = compute_weight(source, target, covers);
    }
    return wgraph;
}

double EdgeWeightCalculator::compute_weight(int u, int v,
                                            std::vector<std::unordered_set<int>>& covers) {
    double struct_sim = compute_structural_similarity(u, v, covers);
    double feature_sim = compute_feature_similarity(u, v);
    double weight = struct_sim * fusion_coefficient + feature_sim * (1 - fusion_coefficient);

    return weight;
}

double EdgeWeightCalculator::compute_feature_similarity(int u, int v) {
    int num_features = graph.get_num_features();
    int num_similar_features = 0;
    const double tolerance = 0.0001;

    if (type == Graph::feature_type::b) {
        for (int i = 0; i < num_features; i++) {
            if (graph.is_missing(u, i) || graph.is_missing(v, i)) {
                continue;
            }

            if (graph.get_bool_feature(u, i) == graph.get_bool_feature(v, i)) {
                num_similar_features++;
            }
        }
    } else if (type == Graph::feature_type::i) {
        for (int i = 0; i < num_features; i++) {
            if (graph.is_missing(u, i) || graph.is_missing(v, i)) {
                continue;
            }

            if (graph.get_int_feature(u, i) == graph.get_int_feature(v, i)) {
                num_similar_features++;
            }
        }
    } else if (type == Graph::feature_type::d) {
        for (int i = 0; i < num_features; i++) {
            if (graph.is_missing(u, i) || graph.is_missing(v, i)) {
                continue;
            }

            double feature_u = graph.get_double_feature(u, i);
            double feature_v = graph.get_double_feature(v, i);
            if (std::abs(feature_u - feature_v) < tolerance) {
                num_similar_features++;
            }
        }
    }

    double mfs = static_cast<double>(num_similar_features) / static_cast<double>(num_features);

    return mfs;
}

double EdgeWeightCalculator::compute_structural_similarity(
    int u, int v, std::vector<std::unordered_set<int>>& covers) {
    std::unordered_set<int> cover_u = covers[u];
    std::unordered_set<int> cover_v = covers[v];

    int intersection_size = 0;
    for (const auto& node : cover_u) {
        if (cover_v.count(node)) {
            intersection_size++;
        }
    }
    int union_size = cover_u.size() + cover_v.size() - intersection_size;

    double mss = static_cast<double>(intersection_size) / static_cast<double>(union_size);

    return mss;
}

std::vector<std::unordered_set<int>> EdgeWeightCalculator::compute_covers(int depth) {
    int num_nodes = graph.get_num_nodes();
    std::vector<std::unordered_set<int>> covers(num_nodes);

    for (int node = 0; node < num_nodes; ++node) {
        std::vector<int> neighbours = graph.get_neighbours(node, depth);
        for (const auto& neighbour : neighbours) {
            covers[node].insert(neighbour);
        }
        covers[node].erase(node);
    }

    return covers;
}