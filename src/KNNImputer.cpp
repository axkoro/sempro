#include "KNNImputer.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Graph.hpp"

KNNImputer::KNNImputer(Graph& graph, int k) : graph(graph), k(k) { run(); }

void KNNImputer::run() {
    
    int num_nodes = graph.get_num_nodes();
    int num_features = graph.get_num_features();

    // for each node in graph if feature is missing take neighbours depth k and take average of
    // features
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_features; j++)
            if (graph.missing[i][j] == true) {
                std::vector<int> neighbours = graph.get_neighbours(i, k);
                double sum = 0;
                int count = 0;
                for (int n : neighbours) {
                    if (graph.missing[n][j] == false) {
                        sum += graph.features[n][j];
                        count++;
                    }
                }
                if (count > 0) {
                    graph.features[i][j] = sum / count;
                } else {
                    graph.features[i][j] = 0;  // TODO: take average of all the nodes that have the
                                         // feature
                }
            }
    }
}