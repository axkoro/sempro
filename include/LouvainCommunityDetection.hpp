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
    struct CommunityStats {  // TODO: change weights to ints?
        // double internal_weight{0.0};  // sum of all internal edges
        double total_weight{0.0};  // sum of all edges (including edges to other communities)
    };

    MinimalGraph current_graph;
    std::vector<int> node_to_community;  // maps each node to a community
    std::unordered_map<long, double>
        community_connections;  // maps each community edge (encoded as a long) to a weight
    std::vector<CommunityStats> community_stats;

    void initialize();
    bool optimize_modularity();
    void aggregate_communities();

    double calculate_modularity_gain(int node, int target_comm);
    void move_node(int node, int old_comm, int new_comm);
    void update_community_connection(int comm1, int comm2, double weight_delta);
    static long encode_community_pair(int comm1, int comm2);
    static std::pair<int, int> decode_community_pair(long comm_pair);
};
