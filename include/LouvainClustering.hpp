#ifndef LOUVAIN_CLUSTERING_HPP
#define LOUVAIN_CLUSTERING_HPP

#include "Graph.hpp"
#include <vector>
#include <unordered_map>

class LouvainClustering {
public:
    explicit LouvainClustering(Graph& graph);

    // Runs the Louvain clustering algorithm and returns the community assignments.
    std::vector<int> run();

private:
    Graph& graph;

    // Computes modularity gain for moving a node to a new community
    double modularity_gain(int node, int community, const std::vector<int>& community_assignment);

    // Aggregates the graph by collapsing communities into supernodes
    void aggregate_graph(std::vector<int>& community_assignment);
};


#endif