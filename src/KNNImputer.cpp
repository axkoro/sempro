#include "KNNImputer.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Graph.hpp"

KNNImputer::KNNImputer(Graph& graph) : graph(graph) {}

void KNNImputer::run() {
    int num_nodes = graph.get_num_nodes();
    int num_features = graph.get_num_features();

    // for each node in graph if feature is missing take neighbours depth k and take average of
    // features
    for (int node = 0; node < num_nodes; node++) {
        std::vector<int> neighbours = graph.get_neighbours(node, k);
        for (int feature = 0; feature < num_features; feature++) {
            if (graph.missing[node][feature] == true) {
                double sum = 0;
                int count = 0;
                for (int neighbour : neighbours) {
                    if (graph.missing[neighbour][feature] == false) {
                        sum += graph.features[neighbour][feature];
                        count++;
                    }
                }

                // TODO: set "imputed" flag, so that future imputations might ignore this feature
                if (count > 0) {
                    graph.features[node][feature] = sum / count;
                } else {
                    for (int nodeglobal = 0; nodeglobal < num_nodes; nodeglobal++) {
                        if (graph.missing[nodeglobal][feature] == false) {
                            sum += graph.features[nodeglobal][feature];
                            count++;
                        }
                    }
                    graph.features[node][feature] = sum / count;
                }
            }
        }
    }
}

void KNNImputer::set_depth(int k) { this->k = k; }