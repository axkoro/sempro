#include "LouvainCommunityDetection.hpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <unordered_set>

#include "MinimalGraph.hpp"

LouvainCommunityDetection::LouvainCommunityDetection(const Graph& g) : current_graph(g) {
    initialize();
}

void LouvainCommunityDetection::initialize() {
    int num_nodes = current_graph.get_num_nodes();
    node_to_community.resize(num_nodes);
    community_stats.resize(num_nodes);

    // Initialize each node to its own community
    std::iota(node_to_community.begin(), node_to_community.end(), 0);

    for (int node = 0; node < num_nodes; node++) {
        // Initialize community connections
        for (auto pair : current_graph.get_neighbours(node)) {
            long comm_pair = encode_community_pair(node, pair.first);
            community_connections[comm_pair]++;
        }

        community_stats[node].total_weight = current_graph.get_degree(node);
    }
}

std::vector<int> LouvainCommunityDetection::execute() {
    bool improvement = true;
    while (improvement) {
        improvement = optimize_modularity();
        if (!improvement) break;
        aggregate_communities();
    }
    return node_to_community;
}

bool LouvainCommunityDetection::optimize_modularity() {
    bool improved = false;

    bool local_improvement;
    do {
        local_improvement = false;

        // Process nodes in random order
        std::vector<int> nodes(current_graph.get_num_nodes());
        std::iota(nodes.begin(), nodes.end(), 0);
        std::shuffle(nodes.begin(), nodes.end(), std::random_device{});

        for (int node : nodes) {
            int current_comm = node_to_community[node];

            // Find neighbouring communities
            std::unordered_set<int> neighbour_communities;
            auto neighbours = current_graph.get_neighbours(node);
            for (auto pair : neighbours) {
                neighbour_communities.insert(node_to_community[pair.first]);
            }

            // Find best community
            int best_community = current_comm;
            double best_gain = 0.0;

            for (int candidate_comm : neighbour_communities) {
                double gain = calculate_modularity_gain(node, candidate_comm);
                if (gain > best_gain) {
                    best_gain = gain;
                    best_community = candidate_comm;
                }
            }

            if (best_community != current_comm && best_gain > 1e-14) {
                move_node(node, current_comm, best_community);
                local_improvement = true;
                improved = true;
            }
        }
    } while (local_improvement);

    return improved;
}

double LouvainCommunityDetection::calculate_modularity_gain(int node, int target_comm) {
    double weight_to_community = 0.0;

    for (auto pair : current_graph.get_neighbours(node)) {
        int neighbour = pair.first;
        double weight = pair.second;

        if (node_to_community[neighbour] == target_comm) {
            weight_to_community += weight;
        }
    }

    double m2 = 2.0 * current_graph.get_num_edges();
    double ki = current_graph.get_degree(node);
    double ki_in = weight_to_community;
    double sigma_tot = community_stats[target_comm].total_weight;

    return (ki_in - (sigma_tot * ki) / m2);  // TODO: verify
}

void LouvainCommunityDetection::move_node(int node, int old_comm, int new_comm) {
    node_to_community[node] = new_comm;

    double node_weight = current_graph.get_degree(node);
    community_stats[old_comm].total_weight -=
        node_weight;  // TODO: does this remove one edge too much (the connection remains, right?)
    community_stats[new_comm].total_weight +=
        node_weight;  // TODO: does this count the connecting edge between the communities twice?

    for (auto pair : current_graph.get_neighbours(node)) {
        int neighbour = pair.first;
        int weight = pair.second;

        update_community_connection(old_comm, node_to_community[neighbour], -weight);
        update_community_connection(new_comm, node_to_community[neighbour], weight);
    }
}

void LouvainCommunityDetection::aggregate_communities() {
    int num_nodes = current_graph.get_num_nodes();
    std::vector<int> new_community_ids(num_nodes, -1);
    int next_community_id = 0;

    // Create mapping of old to new (consecutive) community IDs
    for (int i = 0; i < num_nodes; i++) {
        int old_comm = node_to_community[i];
        if (new_community_ids[old_comm] == -1) {
            new_community_ids[old_comm] = next_community_id++;
        }
    }

    // Update node community assignments
    for (int i = 0; i < num_nodes; i++) {
        node_to_community[i] = new_community_ids[node_to_community[i]];
    }

    // Update community_connections to match renaming
    {
        std::unordered_map<long, double> new_community_connections;
        for (const auto& p : community_connections) {
            long old_key = p.first;
            double weight = p.second;

            std::pair<int, int> comms = decode_community_pair(old_key);

            int comm1 = new_community_ids[comms.first];
            int comm2 = new_community_ids[comms.second];

            long long new_key = encode_community_pair(comm1, comm2);
            new_community_connections[new_key] += weight;
        }
        community_connections = std::move(new_community_connections);
    }

    MinimalGraph new_graph(next_community_id);

    // Fill it with the aggregated edges from community_connections
    for (auto& pair : community_connections) {
        long long key = pair.first;
        double weight = pair.second;

        std::pair<int, int> comms = decode_community_pair(key);

        new_graph.add_edge(comms.first, comms.second, weight);
    }

    current_graph = std::move(new_graph);

    // Rebuild community_stats to match the new graph
    community_stats.clear();
    community_stats.resize(next_community_id);
    for (int comm = 0; comm < next_community_id; comm++) {
        community_stats[comm].total_weight = current_graph.get_degree(comm);
    }
}

long LouvainCommunityDetection::encode_community_pair(int comm1, int comm2) {
    if (comm1 > comm2) std::swap(comm1, comm2);
    return (long)comm1 << 32 | (long)comm2;
}

std::pair<int, int> LouvainCommunityDetection::decode_community_pair(long comm_pair) {
    int c1 = (int)(comm_pair >> 32);
    int c2 = (int)(comm_pair & 0xffffffff);
    return {c1, c2};
}

void LouvainCommunityDetection::update_community_connection(int comm1, int comm2,
                                                            double weight_delta) {
    long pair = encode_community_pair(comm1, comm2);
    community_connections[pair] += weight_delta;
    if (std::abs(community_connections[pair]) <
        1e-10) {  // TODO: remove when switching to int weights
        community_connections.erase(pair);
    }
}

double LouvainCommunityDetection::get_modularity() const {
    // Implementation of modularity calculation
    // Q = 1/(2m) * sum_(i,j) [A_ij - (k_i*k_j)/(2m)] * delta(c_i,c_j)
    double Q = 0.0;

    // TODO:

    return Q;
}
