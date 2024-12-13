#include "KNNImputer.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Graph.hpp"

KNNImputer::KNNImputer(Graph& g) : Imputer(g) {}

void KNNImputer::set_depth(int k) { this->k = k; }

void KNNImputer::run() {
    int num_nodes = graph.get_num_nodes();
    int num_features = graph.get_num_features();

    // for each node in graph if feature is missing take neighbours depth k and take average of
    // features
    for (int node = 0; node < num_nodes; node++) {
        std::vector<int> neighbours = graph.get_neighbours(node, k);
        std::vector<int> missing_features = graph.get_missing_features(node);

        for (auto&& feature : missing_features) {
            double sum = 0;
            int count = 0;
            for (int neighbour : neighbours) {
                if (!graph.is_missing(neighbour, feature)) {
                    sum += graph.get_feature(neighbour, feature);
                    count++;
                }
            }

            if (count > 0) {
                graph.set_feature(node, feature, sum / count);
            } else {  // neighbourhood also doesn't have this feature
                double global_avg = compute_global_average(graph, feature);
                graph.set_feature(node, feature, global_avg);
            }
            graph.set_missing(node, feature, false);
        }
    }
}

double compute_global_average(Graph& graph, int feature) {
    double sum = 0;
    int count = 0;
    for (int node = 0; node < graph.get_num_nodes(); node++) {
        if (!graph.is_missing(node, feature)) {
            sum += graph.get_feature(node, feature);
            count++;
        }
    }
    return sum / count;
}