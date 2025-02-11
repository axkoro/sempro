#include "EdgeWeightCalculator.hpp"
#include "WeightedGraph.hpp"
#include <unordered_set>
#include <queue>
#include <iostream>

EdgeWeightCalculator::EdgeWeightCalculator(Graph& graph, double fusion_coefficient)
    : graph(graph), fusion_coefficient(fusion_coefficient){}
    

WeightedGraph EdgeWeightCalculator::generate_weighted_graph() {
    WeightedGraph wgraph(graph);
    std::vector<std::unordered_set<int>> covers = compute_covers(wgraph);


    for (auto it = wgraph.begin(); it != wgraph.end(); ++it) {
        auto [source, target] = it.get_edge();
        *it = compute_weight(source, target,covers);
    }
    return wgraph;
}

double EdgeWeightCalculator::compute_weight(int u, int v,std::vector<std::unordered_set<int>> covers) {
    double factor = fusion_coefficient;
    double struct_sim = compute_structural_similarity(u, v,covers);
    double feature_sim = compute_feature_similarity(u, v);
    double weight = struct_sim * factor + feature_sim * (1 - factor);
    return weight;
}

double EdgeWeightCalculator::compute_feature_similarity(int u, int v) {
    return 1.0;
}

double EdgeWeightCalculator::compute_structural_similarity(int u, int v,std::vector<std::unordered_set<int>> covers) {
    //Calculate MAS 
   std::unordered_set<int> cover_u = covers[u];
   std::unordered_set<int> cover_v = covers[v];
   //compute union and intersection
    int intersection_size = 0;
    for (const auto& elem : cover_u) {
        if (cover_v.count(elem)) {
            intersection_size++;
        }
    }
    int union_size = cover_u.size() + cover_v.size() - intersection_size;
    //calculate MSS

    double mss = static_cast<double>(intersection_size) / static_cast<double>(union_size);
    
    return mss;
}
std::vector<std::unordered_set<int>> EdgeWeightCalculator::compute_covers(WeightedGraph& wgraph){

    int num_nodes = wgraph.get_num_nodes();
    std::vector<std::unordered_set<int>> covers(num_nodes);

    for (int node = 0; node < num_nodes; ++node) {
        std::vector<WeightedGraph::Edge> edges = wgraph.get_edges(node, 2);
        for (const auto& edge : edges) {
            int neighbor = edge.target;
            if (neighbor != node) {
                covers[node].insert(neighbor);
            }
        }
    }

  return covers;
}