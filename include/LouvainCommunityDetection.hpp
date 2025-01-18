#pragma once
#include <unordered_map>
#include <vector>

#include "Graph.hpp"
#include "MinimalGraph.hpp"

class LouvainCommunityDetection {
   public:
    explicit LouvainCommunityDetection(const Graph &g);

    std::vector<int> execute();

    double get_modularity() const;

   private:
    int max_phase1_iterations = 5;  // TODO: make configurable

    MinimalGraph current_graph;

    // maps each node to a community
    std::vector<int> node_to_community;

    // maps each community edge (encoded as a long) to a weight
    std::unordered_map<long, double> community_connections;

    // TODO: change weight values from double to int

    // stores the sum of all edge weights within a given community (used for modularity
    // calculations)
    std::vector<double> community_total_weights;

    double total_edge_weight;

    void initialize();
    bool optimize_modularity();
    void aggregate_communities();

    double calculate_modularity_gain(int node, int target_comm);
    void move_node(int node, int old_comm, int new_comm);
    void update_community_connection(int comm1, int comm2, double weight_delta);
    static long encode_community_pair(int comm1, int comm2);
    static std::pair<int, int> decode_community_pair(long comm_pair);
};
