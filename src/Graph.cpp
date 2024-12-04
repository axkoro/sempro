#include "Graph.hpp"

#include <fstream>
#include <sstream>

Graph::Graph(std::string edges_path, std::string features_path) {
    num_nodes = getNumNodes(edges_path);
    readEdgesFromFile(edges_path);
    // readFeaturesFromFile(features_path);
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
        std::istringstream iss(line);
        int a, b;
        if (!(iss >> a >> b)) {
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

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int a, b;
        if (!(iss >> a >> b)) {
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

// Assumptions about file format: edges ordered descending (a > b), lines ordered ascending, newline
// after last edge
int getNumNodes(std::string edges_path) {
    std::ifstream file(edges_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    // iterate backwards to find last line
    file.seekg(-2, file.end);  // -2 because -1 would always be newline (see assumptions above)
    size_t pos = file.tellg();
    while (file.get() != '\n') {
        file.seekg(--pos);
    }

    // get first node from last line
    std::string last_line;
    getline(file, last_line);
    std::istringstream iss(last_line);
    int first_node;
    iss >> first_node;

    file.close();

    return first_node;
}