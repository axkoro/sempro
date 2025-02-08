#include "NodeDistribution.hpp"

#include <cmath>

NodeDistribution::NodeDistribution(const std::vector<std::vector<int>> &random_walks,
                                   int num_nodes) {
    double smoothing_exponent = 0.75;

    std::vector<int> node_occurrences(num_nodes, 0);
    for (auto &&walk : random_walks) {
        for (auto &&node : walk) {
            node_occurrences[node]++;
        }
    }

    // calculated (smoothed) relative frequencies
    std::vector<double> relative_frequencies(num_nodes);
    double total = 0.0;
    for (int i = 0; i < num_nodes; i++) {
        double smoothed_frequency = std::pow(node_occurrences[i], smoothing_exponent);
        relative_frequencies[i] = smoothed_frequency;
        total += smoothed_frequency;
    }
    for (int i = 0; i < num_nodes; i++) {
        relative_frequencies[i] /= total;
    }

    // TODO: generate alias table
}