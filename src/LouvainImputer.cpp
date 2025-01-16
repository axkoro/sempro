// LouvainFeatureImputation.cpp
#include "LouvainImputer.hpp"
#include "KNNImputer.hpp"
#include <numeric>
#include <iostream>

// Constructor: Initializes the feature imputation class
LouvainImputer::LouvainImputer(Graph& graph, const std::vector<int>& communities)
    : graph(graph), communities(communities) {}

// Compute mean feature values for each community
std::unordered_map<int, std::vector<double>> LouvainImputer::compute_community_average() {
    std::unordered_map<int, std::vector<double>> community_sums;
    std::unordered_map<int, std::vector<int>> community_counts;

    // Initialize sums and counts
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        int community = communities[node];
        if (community_sums.find(community) == community_sums.end()) {
            community_sums[community] = std::vector<double>(graph.get_num_features(), 0.0);
            community_counts[community] = std::vector<int>(graph.get_num_features(), 0);
        }
        for (int feature = 0; feature < graph.get_num_features(); ++feature) {
            if (!graph.is_missing(node, feature)) {
                community_sums[community][feature] += graph.get_feature(node, feature);
                community_counts[community][feature]++;
            }
        }
    }
    // Calculate means
    for (auto& [community, sums] : community_sums) {
        for (int feature = 0; feature < sums.size(); ++feature) {
            sums[feature] /= community_counts[community][feature];
        }
    }

    return community_sums;
}

// Perform feature imputation
void LouvainImputer::run() {
    auto community_means = compute_community_average();

    // Impute missing features with community means
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        int community = communities[node];
        for (int feature = 0; feature < graph.get_num_features(); ++feature) {
            if (graph.is_missing(node, feature)) {
                graph.set_feature(node, feature, community_means[community][feature]);
                graph.set_missing(node, feature, false); // Mark as imputed
            }
        }
    }
}


