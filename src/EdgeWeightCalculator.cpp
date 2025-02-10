#include "EdgeWeightCalculator.hpp"
#include "WeightedGraph.hpp"
#include <unordered_set>

EdgeWeightCalculator::EdgeWeightCalculator(Graph& graph, double fusion_coefficient)
    : graph(graph), fusion_coefficient(fusion_coefficient) {}

WeightedGraph EdgeWeightCalculator::generate_weighted_graph() {
    WeightedGraph wgraph(graph);
    for (auto it = wgraph.begin(); it != wgraph.end(); ++it) {
        auto [source, target] = it.get_edge();
        *it = compute_weight(source, target);
    }
    return wgraph;
}

double EdgeWeightCalculator::compute_weight(int u, int v) {
    double factor = fusion_coefficient;
    double struct_sim = compute_structural_similarity(u, v);
    double feature_sim = compute_feature_similarity(u, v);
    double weight = struct_sim * factor + feature_sim * (1 - factor);
    return weight;
}

double EdgeWeightCalculator::compute_feature_similarity(int u, int v) {
    return 1.0;
}

double EdgeWeightCalculator::compute_structural_similarity(int u, int v) {
    
    return 1.0;
}
std::vector<std::vector<int>> EdgeWeightCalculator::compute_covers(WeightedGraph& wgraph){
    //compute all covers
}