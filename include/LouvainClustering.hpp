#ifndef LOUVAIN_CLUSTERING_HPP
#define LOUVAIN_CLUSTERING_HPP

#include "Graph.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <numeric>

class LouvainClustering {
public:
    // Constructor
    LouvainClustering(const Graph& graph);

    // Method to execute Louvain Clustering
    std::vector<int> execute(int max_iter = 5);

private:
    // Reference to the input graph
    const Graph& graph;

    // Current modularity
    double current_modularity;

    // Node to community mapping
    std::vector<int> node_to_community;

    // Community to total degree mapping
    std::unordered_map<int, int> community_totals;

    // Helper functions
    void initialize();
    double calculate_modularity(const std::vector<int>& community_assignment) const;
    void move_node_to_best_community(int node);
    void rebuild_graph();
};

#endif // LOUVAIN_CLUSTERING_HPP