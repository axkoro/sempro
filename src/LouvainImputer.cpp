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
void LouvainImputer::run() {
    // Impute missing features with community means
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        int community = communities[node];

        for (int feature = 0; feature < graph.get_num_features(); ++feature) {
            if (graph.is_missing(node, feature)) {
                double average = compute_community_average(node, feature);

                // Set the imputed value
                if (type == b) {
                    graph.set_bool_feature(node, feature, average);
                } else if (type == d) {
                    graph.set_double_feature(node, feature, average);
                } else if (type == i) {
                    graph.set_int_feature(node, feature, average);
                }
                graph.set_missing(node, feature, false); // Mark as imputed
            }
        }
    }
}

double LouvainImputer::compute_community_average(int node, int feature) {
    // Compute the average feature value for the community or the global average if not available
    int community = communities[node];
    double sum = 0.0;
    int count = 0;

    // Calculate community average on demand
    for (int other_node = 0; other_node < graph.get_num_nodes(); ++other_node) {
        if (communities[other_node] == community && !graph.is_missing(other_node, feature)) {
            if (type == b) {
                sum += graph.get_bool_feature(other_node, feature);
            } else if (type == d) {
                sum += graph.get_double_feature(other_node, feature);
            } else if (type == i) {
                sum += graph.get_int_feature(other_node, feature);
            }
            count++;
        }
    }

    if (count == 0) {
        // Calculate global average if community average is not available
        if (type == b) {
            return compute_global_average_bool(graph, feature);
        } else if (type == d) {
            return compute_global_average_double(graph, feature);
        } else if (type == i) {
            return compute_global_average_int(graph, feature);
        }
    }

    return sum / count;
}
