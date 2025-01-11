#ifndef LOUVAIN_CLUSTERING_HPP
#define LOUVAIN_CLUSTERING_HPP

#include "Graph.hpp"
#include <vector>
#include <unordered_map>

class LouvainClustering {
public:
    // Constructor: Takes a reference to the graph object
    LouvainClustering(Graph& graph);

    // Main method to execute Louvain clustering
    std::vector<int> run();

private:
    Graph& graph;                          // Reference to the input graph
    std::vector<int> communities;          // Stores the community assignment of each node
    std::unordered_map<int, double> community_totals; // Total degree of each community

    // Initialize each node to its own community
    void initialize_communities();

    // Phase 1: Optimize modularity by moving nodes between communities
    bool phase_one();

    // Phase 2: Aggregate communities into a new graph
    void phase_two();

    // Calculate the modularity gain of moving a node to a different community
    double modularity_gain(int node, int community, double edge_weight);

    // Update community assignments and totals
    void update_community(int node, int old_community, int new_community);

    // Aggregate the graph by merging nodes in the same community
    void aggregate_communities(std::vector<int>& new_offsets, std::vector<int>& new_edges);
};

#endif // LOUVAIN_CLUSTERING_HPP
