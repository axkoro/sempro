#include "LouvainClustering.hpp"
#include <iostream>
#include <limits>
#include <algorithm>

// Constructor: Initializes the clustering with the provided graph
LouvainClustering::LouvainClustering(const Graph& graph) : graph(graph), current_modularity(-std::numeric_limits<double>::max()) {
    initialize();
}

// Initialization
// Sets up the node-to-community mapping and calculates the initial community totals
void LouvainClustering::initialize() {
    int num_nodes = graph.get_num_nodes();
    node_to_community.resize(num_nodes);
    community_totals.clear();

    // Initially, each node is its own community
    for (int i = 0; i < num_nodes; ++i) {
        node_to_community[i] = i;
        community_totals[i] = graph.get_degree(i); // Total degree of the node is its own community total
    }
}


//Calculate Modularity
// Computes the modularity of the current partition
// Formula: Q = (1 / (2m)) * sum(Aij - (ki * kj) / (2m))
// Where Aij is the adjacency matrix, ki and kj are degrees, and m is total edge count
double LouvainClustering::calculate_modularity(const std::vector<int>& community_assignment) const {
    double modularity = 0.0;
    int m = 0;
    for (int i = 0; i < graph.get_num_nodes(); ++i) {
        m += graph.get_degree(i);
    }
    m /= 2; // Each edge is counted twice

    for (int i = 0; i < graph.get_num_nodes(); ++i) {
        for (int j = 0; j < graph.get_num_nodes(); ++j) {
            if (community_assignment[i] == community_assignment[j]) {
                int Aij = graph.has_edge(i, j);
                double ki = graph.get_degree(i);
                double kj = graph.get_degree(j);
                modularity += Aij - (ki * kj) / (2.0 * m);
            }
        }
    }
    return modularity / (2.0 * m);
}

