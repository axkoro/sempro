#pragma once
#include <unordered_map>
#include <vector>
#include <cstdint>

#include "DynamicMinimalGraph.hpp"
#include "Graph.hpp"

class Louvain {
   public:
    explicit Louvain(const Graph &g);

    std::vector<int> execute();

    double get_modularity() const;

   private:
    bool executed = false;
    int max_iterations = 50;

    DynamicMinimalGraph current_graph;

    // maps each node to a community
    std::vector<int> total_node_to_community;  // maps nodes from the ORIGINAL graph to communities
    std::vector<int> local_node_to_community;  // maps nodes from the CURRENT graph to communities

    // maps each community edge (encoded as a uint64_t) to a weight
    std::unordered_map<uint64_t, int> community_connections;

    // stores the sum of all edge weights within a given community (used for modularity
    // calculations)
    std::vector<int> community_total_weights;

    int total_edge_weight;

    void set_max_iterations(int max_iterations);

    void initialize();
    bool optimize_modularity();
    void aggregate_communities();

    double calculate_modularity_gain(int node, int target_comm);
    void move_node(int node, int old_comm, int new_comm);
    void update_community_connection(int comm1, int comm2, int weight_delta);
    static uint64_t encode_community_pair(int comm1, int comm2);
    static std::pair<int, int> decode_community_pair(uint64_t comm_pair);
};
