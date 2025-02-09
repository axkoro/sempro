#include "RandomWalkGenerator.hpp"
#include <iostream>
#include <random>
#include <numeric> // For std::partial_sum
#include <algorithm> // For std::upper_bound

RandomWalkGenerator::RandomWalkGenerator(const StaticMinimalGraph& graph, int walk_length, int num_walks, int seed)
    : graph(graph), walk_length(walk_length), num_walks(num_walks) {
    if (seed == -1) {
        std::random_device rd;
        rng.seed(rd());
    } else {
        rng.seed(seed);
    }
}

void RandomWalkGenerator::set_seed(int seed) {
    rng.seed(seed);
}

int RandomWalkGenerator::select_weighted_random_neighbor(const std::vector<Edge>& neighbors, std::mt19937& rng) {
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
    std::vector<std::vector<int>> walks;
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        for (int i = 0; i < num_walks; ++i) {
            walks.push_back(perform_walk(node));
        }
    }
    return walks; // TODO: benchmark if returning with move semantics is quicker
}

std::vector<int> RandomWalkGenerator::perform_walk(int start_node) {
    std::vector<int> walk(walk_length);
    walk[0] = start_node;
    int current_node = start_node;
    const auto& neighbors = graph.get_neighbours(current_node);

    if (neighbors.empty()) return walk;  // Early exit if start node has no neighbors

    for (int i = 1; i < walk_length; ++i) {
        const auto& neighbors = graph.get_neighbours(current_node);
        current_node = select_weighted_random_neighbor(neighbors, rng);
        walk[i] = current_node;
    }
    return walk; // TODO: benchmark if using move semantics is quicker
}
