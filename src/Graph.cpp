#include "Graph.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

Graph::Graph(std::string edges_path, std::string features_path) {
    num_nodes = getNumNodes(edges_path);
    readEdgesFromFile(edges_path);
    readFeaturesFromFile(features_path);
}

// Graph: undirected, no loops (A->A)
// Edge file format: sorted, every edge is "descending" (a b -> b < a)
void Graph::readEdgesFromFile(std::string edges_path) {
    std::ifstream file(edges_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    // Populate offsets
    std::vector<int> edge_counts(num_nodes);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream line_stream(line);
        int a, b;
        if (!(line_stream >> a >> b)) {
            break;
        }

        ++edge_counts[a];
        ++edge_counts[b];
    }

    offsets = std::vector<int>(num_nodes + 1);
    for (int i = 1; i < offsets.size(); i++) {
        offsets[i] = offsets[i - 1] + edge_counts[i - 1];
    }

    // Populate edges
    int num_edges = offsets[num_nodes];
    edges = std::vector<int>(num_edges);

    file.clear();  // reset EOF flag, so that iteration works again
    file.seekg(0);
    while (std::getline(file, line)) {
        std::istringstream line_stream(line);
        int a, b;
        if (!(line_stream >> a >> b)) {
            break;
        }

        int next_edge_a = offsets[a + 1] - edge_counts[a];
        edges[next_edge_a] = b;
        --edge_counts[a];

        int next_edge_b = offsets[b + 1] - edge_counts[b];
        edges[next_edge_b] = a;
        --edge_counts[b];
    }

    file.close();
}

void Graph::readFeaturesFromFile(std::string features_path) {
    int num_features = getNumFeatures(features_path);

    std::ifstream file(features_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream line_stream(line);

        int node;
        line_stream >> node;

        std::vector<double> node_features(num_features);
        for (int j = 0; j < num_features; j++) {
            // TODO: missing features
            line_stream >> node_features[j];
        }

        features[node] = node_features;  // TODO: does it copy or just set a reference? (efficient?)
    }

    for (int i = 0; i < num_nodes; i++) {
    }

    file.close();
}

// Prints unique edges
void Graph::printEdges() {
    for (int i = 0; i < num_nodes; i++) {
        for (int j = offsets[i]; j < offsets[i + 1]; j++) {
            int neighbour = edges[j];
            if (neighbour <= i) {  // to only print unique edges (in descending order)
                std::cout << i << "\t" << neighbour << "\n";
            }
        }
    }
}

// Features file format: "1 0.93, '#', -3.2 2" + lines ordered ascending, newline after last line
int getNumNodes(std::string features_path) {
    std::ifstream file(features_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    // iterate backwards to find last line
    file.seekg(-2, file.end);  // -2 because -1 would always be newline (see assumptions above)
    size_t pos = file.tellg();
    while (file.get() != '\n') {
        file.seekg(--pos);
    }

    // get node from last line
    std::string last_line;
    getline(file, last_line);
    std::istringstream line_stream(last_line);
    int node;
    line_stream >> node;

    file.close();

    int num_nodes = node + 1;  // assuming naming starts at 0

    return num_nodes;
}

//
int getNumFeatures(std::string features_path) {
    std::ifstream file(features_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::string line;
    std::getline(file, line);

    int num_features = std::count(line.begin(), line.end(), ',') +
                       2;  // + 1 because we include the label as a feature

    return num_features;
}