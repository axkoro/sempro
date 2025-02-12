#include "RandomWalkGenerator.hpp"

#include <algorithm>  // For std::upper_bound
#include <iostream>
#include <random>

RandomWalkGenerator::RandomWalkGenerator(const WeightedGraph& graph, int walk_length, int num_walks,
                                         int seed)
    : graph(graph), walk_length(walk_length), num_walks(num_walks) {
    if (seed == -1) {
        std::random_device rd;
        rng.seed(rd());
    } else {
        rng.seed(seed);
    }
}

int RandomWalkGenerator::select_weighted_random_neighbor(
    const std::vector<WeightedGraph::Edge>& neighbors, std::mt19937& rng) {
    std::vector<double> cumulative_weights(neighbors.size());
    cumulative_weights[0] = neighbors[0].weight;

    for (size_t j = 1; j < neighbors.size(); ++j) {
        cumulative_weights[j] = cumulative_weights[j - 1] + neighbors[j].weight;
    }

    std::uniform_real_distribution<> distrib(0, cumulative_weights.back());
    double random_value = distrib(rng);

    auto it = std::upper_bound(cumulative_weights.begin(), cumulative_weights.end(), random_value);
    int index = std::distance(cumulative_weights.begin(), it);

    return neighbors[index].target;
}

std::vector<std::vector<int>> RandomWalkGenerator::generate_walks() {
    int num_nodes = graph.get_num_nodes();
    std::vector<std::vector<int>> walks(num_nodes * num_walks, std::vector<int>(walk_length));
    int next_walk_idx = 0;
    for (int node = 0; node < num_nodes; ++node) {
        for (int i = 0; i < num_walks; ++i) {
            walks[next_walk_idx] = perform_walk(node);
            next_walk_idx++;
        }
    }
    return walks;  // TODO: benchmark if returning with move semantics is quicker
}

std::vector<int> RandomWalkGenerator::perform_walk(int start_node) {
    std::vector<int> walk(walk_length);
    walk[0] = start_node;
    int current_node = start_node;

    std::vector<WeightedGraph::Edge> neighbors = graph.get_edges(current_node);
    if (neighbors.empty()) return walk;

    for (int i = 1; i < walk_length; ++i) {
        std::vector<WeightedGraph::Edge> neighbors = graph.get_edges(current_node);
        current_node = select_weighted_random_neighbor(neighbors, rng);
        walk[i] = current_node;
    }
    return walk;  // TODO: benchmark if using move semantics is quicker
}
