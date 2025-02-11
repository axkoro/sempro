#include "EdgeWeightCalculator.hpp"
#include "WeightedGraph.hpp"
#include <unordered_set>
#include <queue>
#include <iostream>

EdgeWeightCalculator::EdgeWeightCalculator(Graph& graph, double fusion_coefficient,std::string type)
    : graph(graph), fusion_coefficient(fusion_coefficient),type(type){}
    

WeightedGraph EdgeWeightCalculator::generate_weighted_graph() {
    WeightedGraph wgraph(graph);
    std::vector<std::unordered_set<int>> covers = compute_covers(wgraph);
    

    for (auto it = wgraph.begin(); it != wgraph.end(); ++it) {
        auto [source, target] = it.get_edge();
        *it = compute_weight(source, target,covers,wgraph,type);
    }
    return wgraph;
}

double EdgeWeightCalculator::compute_weight(int u, int v,std::vector<std::unordered_set<int>> covers, WeightedGraph& wgraph, std::string type) {
    double factor = fusion_coefficient;
    double struct_sim = compute_structural_similarity(u, v,covers);
    double feature_sim = compute_feature_similarity(u, v, wgraph, type);
    double weight = struct_sim * factor + feature_sim * (1 - factor);
    return weight;
}

double EdgeWeightCalculator::compute_feature_similarity(int u, int v,WeightedGraph& wgraph, std::string type) {
    int num_features = wgraph.get_num_features();
    int same_features = 0;
    const double epsilon = 0.0001; 

    if (type == "bool"){
        for(int i = 0;i<num_features;i++){
            if(wgraph.get_bool_feature(u,i)==wgraph.get_bool_feature(v,i)){
                same_features++;
            }
        }
    }
    if (type == "int"){
        for(int i = 0;i<num_features;i++){
            if(wgraph.get_int_feature(u,i)==wgraph.get_int_feature(v,i)){
                same_features++;
            }
        }
    }
    if (type == "double") {
        for (int i = 0; i < num_features; i++) {
            double feature_u = wgraph.get_double_feature(u, i);
            double feature_v = wgraph.get_double_feature(v, i);
            if (std::fabs(feature_u - feature_v) < epsilon) {
                same_features++;
            }
        }
    }

    return static_cast<double>(same_features)/ static_cast<double>(num_features);
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