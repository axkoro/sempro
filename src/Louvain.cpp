#include "Louvain.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <unordered_set>

Louvain::Louvain(const Graph& g, const LouvainConfig& config) : current_graph(g), config(config) {
    total_edge_weight = g.get_num_edges();
    initialize();
}

void Louvain::initialize() {
    int num_nodes = current_graph.get_num_nodes();
    total_node_to_community.resize(num_nodes);
    community_total_weights.resize(num_nodes);

    // Initialize each node to its own community
    std::iota(total_node_to_community.begin(), total_node_to_community.end(), 0);

    for (int node = 0; node < num_nodes; node++) {
        // Initialize community connections
        for (auto pair : current_graph.get_neighbors(node)) {
            int neighbor = pair.first;
            if (neighbor >= node) {  // prevent double counting of edges
                uint64_t comm_pair = encode_community_pair(node, neighbor);
                community_connections.emplace(comm_pair, 1);  // initial graph is unweighted
            }
        }

        community_total_weights[node] = current_graph.get_degree(node);
    }
}

std::vector<int> Louvain::execute() {
    if (executed) throw std::runtime_error("Louvain has already been executed.");
    executed = true;

    bool improvement;
    int level = 0;
    do {
        improvement = optimize_modularity();
        if (improvement) aggregate_communities();
        level++;
    } while (improvement && level < config.max_levels);

    return total_node_to_community;
}

bool Louvain::optimize_modularity() {
    bool improved = false;

    local_node_to_community.resize(current_graph.get_num_nodes());
    std::iota(local_node_to_community.begin(), local_node_to_community.end(), 0);

    bool local_improvement;
    int iterations = 0;
    do {
        // std::cout << get_modularity() << std::endl;
        local_improvement = false;

        // Process nodes in random order
        std::vector<int> nodes(current_graph.get_num_nodes());
        std::iota(nodes.begin(), nodes.end(), 0);
        std::shuffle(nodes.begin(), nodes.end(), std::random_device{});

        for (int node : nodes) {
            int current_comm = local_node_to_community[node];

            // Find neighboring communities
            std::unordered_set<int> neighbor_communities;
            auto neighbors = current_graph.get_neighbors(node);
            for (auto pair : neighbors) {
                int neighbor = pair.first;
                neighbor_communities.insert(local_node_to_community[neighbor]);
            }

            // Find best community
            int best_community = current_comm;
            double best_gain = 0;

            for (int candidate_comm : neighbor_communities) {
                double gain = calculate_modularity_gain(node, candidate_comm);
                if (gain > best_gain) {
                    best_gain = gain;
                    best_community = candidate_comm;
                }
            }

            if (best_community != current_comm && best_gain > config.tolerance) {
                move_node(node, current_comm, best_community);
                local_improvement = true;
                improved = true;
            }
        }
        iterations++;
    } while (local_improvement && iterations < config.max_iterations);

    return improved;
}

double Louvain::calculate_modularity_gain(int node, int target_comm) {
    int weight_to_community = 0;

    for (auto pair : current_graph.get_neighbors(node)) {
        int neighbor = pair.first;
        int weight = pair.second;

        if (local_node_to_community[neighbor] == target_comm) {
            weight_to_community += weight;
        }
    }

    double m2 = 2.0 * total_edge_weight;
    double deg = current_graph.get_degree(node);
    double total_community_weight = community_total_weights[target_comm];

    return (weight_to_community - (total_community_weight * deg) / m2);
}

void Louvain::move_node(int node, int old_comm, int new_comm) {
    local_node_to_community[node] = new_comm;

    int node_weight = current_graph.get_degree(node);
    community_total_weights[old_comm] -= node_weight;
    community_total_weights[new_comm] += node_weight;

    for (auto pair : current_graph.get_neighbors(node)) {
        int neighbor = pair.first;
        int weight = pair.second;

        update_community_connection(old_comm, local_node_to_community[neighbor], -weight);
        update_community_connection(new_comm, local_node_to_community[neighbor], weight);
    }
}

void Louvain::aggregate_communities() {
    int num_nodes = current_graph.get_num_nodes();
    std::vector<int> old_to_new_community_ids(num_nodes, -1);
    int next_community_id = 0;

    // Create mapping of old to new (consecutive) community IDs
    for (int i = 0; i < num_nodes; i++) {
        int old_comm = local_node_to_community[i];
        if (old_to_new_community_ids[old_comm] == -1) {
            old_to_new_community_ids[old_comm] = next_community_id++;
        }
    }

    // Update node community assignments
    for (int i = 0; i < num_nodes; i++) {
        local_node_to_community[i] = old_to_new_community_ids[local_node_to_community[i]];
    }

    // Update global node_to_community mapping
    for (int node = 0; node < total_node_to_community.size(); node++) {
        int prev_comm = total_node_to_community[node];
        total_node_to_community[node] = local_node_to_community[prev_comm];
    }

    // Update community_connections to match renaming
    {
        std::unordered_map<uint64_t, int> new_community_connections;
        for (const auto& p : community_connections) {
            uint64_t old_key = p.first;
            int weight = p.second;

            std::pair<int, int> comms = decode_community_pair(old_key);
            int comm1 = old_to_new_community_ids[comms.first];
            int comm2 = old_to_new_community_ids[comms.second];
            uint64_t new_key = encode_community_pair(comm1, comm2);

            if (comm1 == -1 || comm2 == -1) continue;  // means this community doesn't exist anymore

            bool inserted = new_community_connections.try_emplace(new_key, weight)
                                .second;  // won't be inserted if key already exists in map
            if (!inserted) {              // only in case of merged communities (comm1==comm2)
                new_community_connections.at(new_key) += weight;
            }
        }
        community_connections = std::move(new_community_connections);
    }

    DynamicGraph new_graph(next_community_id);

    for (auto& pair : community_connections) {
        uint64_t key = pair.first;
        int weight = pair.second;

        std::pair<int, int> comms = decode_community_pair(key);

        new_graph.add_edge(comms.first, comms.second, weight);
    }

    current_graph = std::move(new_graph);

    // Rebuild community_total_weights to match the new graph
    community_total_weights.clear();
    community_total_weights.resize(next_community_id);
    for (int comm = 0; comm < next_community_id; comm++) {
        community_total_weights[comm] = current_graph.get_degree(comm);
    }
}

uint64_t Louvain::encode_community_pair(int comm1, int comm2) {
    if (comm1 > comm2) std::swap(comm1, comm2);
    return (static_cast<uint64_t>(comm1) << 32) | static_cast<uint64_t>(comm2);
}

std::pair<int, int> Louvain::decode_community_pair(uint64_t comm_pair) {
    uint32_t c1 = static_cast<uint32_t>(comm_pair >> 32);
    uint32_t c2 = static_cast<uint32_t>(comm_pair & 0xFFFFFFFF);
    return {static_cast<int>(c1), static_cast<int>(c2)};
}

void Louvain::update_community_connection(int comm1, int comm2, int weight_delta) {
    uint64_t pair = encode_community_pair(comm1, comm2);
    community_connections[pair] += weight_delta;
    if (community_connections[pair] == 0) community_connections.erase(pair);
}

double Louvain::get_modularity() const {
    int m2 = 2 * total_edge_weight;

    double Q = 0.0;

    for (int comm = 0; comm < community_total_weights.size(); ++comm) {
        int internal_community_weight = 0;
        uint64_t key = encode_community_pair(comm, comm);
        auto it = community_connections.find(key);
        if (it != community_connections.end()) {
            internal_community_weight = it->second;
        }

        int total_community_weight = community_total_weights[comm];

        Q += (internal_community_weight - (total_community_weight * total_community_weight / m2));
    }

    return Q / m2;
}