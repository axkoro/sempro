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

    std::unordered_map<int, double> community_intra_edges;
    std::unordered_map<int, double> community_degrees;

    for (int i = 0; i < graph.get_num_nodes(); ++i) {
        int community = community_assignment[i];
        community_degrees[community] += graph.get_degree(i);
        for (int neighbor : graph.get_neighbours(i)) {
            if (community_assignment[neighbor] == community) {
                community_intra_edges[community] += 1.0;
            }
        }
    }

    for (const auto& [community, intra_edges] : community_intra_edges) {
        double degree_sum = community_degrees[community];
        modularity += (intra_edges / (2.0 * m)) - (degree_sum * degree_sum) / (4.0 * m * m);
    }

    return modularity;
}

void LouvainClustering::move_node_to_best_community(int node) {
    int original_community = node_to_community[node];
    double max_gain = -std::numeric_limits<double>::max();
    int best_community = original_community;

    // Remove the node from its current community
    community_totals[original_community] -= graph.get_degree(node);

    // Calculate the total number of edges in the graph
    int m = 0;
    for (int i = 0; i < graph.get_num_nodes(); ++i) {
        m += graph.get_degree(i);
    }
    m /= 2; // Each edge is counted twice

    // Check all neighboring communities
    std::unordered_map<int, int> neighbor_communities;
    for (int neighbor : graph.get_neighbours(node)) {
        neighbor_communities[node_to_community[neighbor]]++;
    }

    for (const auto& [community, edge_count] : neighbor_communities) {
        double gain = edge_count - (graph.get_degree(node) * community_totals[community]) / (2.0 * m);
        if (gain > max_gain) {
            max_gain = gain;
            best_community = community;
        }
    }

    // Assign the node to the best community
    node_to_community[node] = best_community;
    community_totals[best_community] += graph.get_degree(node);

    // If the best community is the original community, revert the change
    if (best_community != original_community) {
        community_totals[original_community] += graph.get_degree(node);
    }
}

// Rebuild Graph
// Aggregates the graph based on current communities
void LouvainClustering::rebuild_graph() {
    // Placeholder for graph aggregation logic
    // You can implement this part to create a new graph where each community becomes a single node
}

// Execute Louvain Clustering
std::vector<int> LouvainClustering::execute(int max_iter) {
    const double MODULARITY_THRESHOLD = 1e-6;
    bool improvement = true;
    int iteration = 0;

    while (improvement && iteration < max_iter) {
        improvement = false;
        for (int node = 0; node < graph.get_num_nodes(); ++node) {
            move_node_to_best_community(node);
        }

        double new_modularity = calculate_modularity(node_to_community);
        if (new_modularity - current_modularity > MODULARITY_THRESHOLD) {
            improvement = true;
            current_modularity = new_modularity;
        }
        iteration++;
    }

    return node_to_community;
}