#include "LouvainImputer.hpp"
#include "KNNImputer.hpp"
#include <numeric>
#include <iostream>

// Constructors: Initialize the feature imputation class
LouvainImputer::LouvainImputer(GraphBool& graph, const std::vector<int>& communities)
    : graph(graph), communities(communities), type(b) {}

LouvainImputer::LouvainImputer(GraphDouble& graph, const std::vector<int>& communities)
    : graph(graph), communities(communities), type(d) {}

LouvainImputer::LouvainImputer(GraphInt& graph, const std::vector<int>& communities)
    : graph(graph), communities(communities), type(i) {}

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
                if (type == b) {
                    community_sums[community][feature] += graph.get_bool_feature(node, feature);
                } else if (type == d) {
                    community_sums[community][feature] += graph.get_double_feature(node, feature);
                } else if (type == i) {
                    community_sums[community][feature] += graph.get_int_feature(node, feature);
                }
                community_counts[community][feature]++;
            }
        }
    }

    // Calculate means
    for (auto& [community, sums] : community_sums) {
        for (int feature = 0; feature < sums.size(); ++feature) {
            if (community_counts[community][feature] == 0) {
                if (type == b) {
                    sums[feature] = compute_global_average_bool(graph, feature);
                } else if (type == d) {
                    sums[feature] = compute_global_average_double(graph, feature);
                } else if (type == i) {
                    sums[feature] = compute_global_average_int(graph, feature);
                }
            } else {
                sums[feature] /= community_counts[community][feature];
            }
        }
    }

    return community_sums;
}

// Perform feature imputation
void LouvainImputer::run() {
    std::unordered_map<int, std::vector<double>> community_means = compute_community_average();

    // Impute missing features with community means
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        int community = communities[node];

        for (int feature = 0; feature < graph.get_num_features(); ++feature) {
            if (graph.is_missing(node, feature)) {
                if (type == b) {
                    graph.set_bool_feature(node, feature, (community_means[community][feature]));
                } else if (type == d) {
                    graph.set_double_feature(node, feature, community_means[community][feature]);
                } else if (type == i) {
                    graph.set_int_feature(node, feature, (community_means[community][feature]));
                }
                graph.set_missing(node, feature, false); // Mark as imputed
            }
        }
    }
}