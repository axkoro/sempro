#include "Graph.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

/**
 * @brief Default constructor. Creates an empty graph. Used for debugging purposes.
 */
Graph::Graph() : num_nodes(0), num_features(0), offsets(0), edges(0), features(0), missing(0) {}

/**
 * @brief Constructs a graph from adjacency list representation using offsets and edges arrays. Used
 * for debugging purposes.
 *
 * @param offsets Array containing offsets into edges array for each node
 * @param edges Array containing target node IDs for edges
 */
Graph::Graph(std::vector<int>& offsets, std::vector<int>& edges) {
    this->offsets = offsets;
    this->edges = edges;
    num_nodes = offsets.size() - 1;
}

Graph::Graph(std::string edges_path, std::string features_path) {
    num_nodes = parse_node_count(features_path);
    num_features = parse_feature_count(features_path);
    read_edges(edges_path);
    read_features(features_path);
}

int Graph::get_num_nodes() const { return num_nodes; }

int Graph::get_num_features() const { return num_features; }

std::vector<int> Graph::get_offsets() const { return offsets; }

std::vector<int> Graph::get_edges() const { return edges; }

std::vector<double> Graph::get_features(int node) const {
    if (!(is_valid_node(node))) throw std::runtime_error("Node does not exist");
    return features[node];
}

std::vector<bool> Graph::get_missing_features(int node) const {
    if (!(is_valid_node(node))) throw std::runtime_error("Node does not exist");
    return missing[node];
}

std::vector<int> Graph::get_neighbours(int node) const {
    if (!(is_valid_node(node))) throw std::runtime_error("Node does not exist");

    std::vector<int> neighbours(offsets[node + 1] - offsets[node]);
    for (int i = 0; i < neighbours.size(); i++) {
        neighbours[i] = edges[offsets[node] + i];
    }

    return neighbours;
}

std::vector<int> Graph::get_neighbours(int node, int depth) const {
    if (!(is_valid_node(node))) throw std::runtime_error("Node does not exist");

    std::vector<int> neighbours = get_neighbours(node);  // k=1

    int start_last_k = 0;
    int start_this_k = neighbours.size();
    for (int k = 2; k <= depth; k++) {
        for (int i = start_last_k; i < start_this_k; i++) {
            std::vector<int> new_neighbours = get_neighbours(neighbours[i]);
            neighbours.insert(neighbours.end(), new_neighbours.begin(), new_neighbours.end());
        }
        neighbours = remove_duplicates(neighbours);
        start_last_k = start_this_k;
        start_this_k = neighbours.size();
    }

    return neighbours;
}

int Graph::get_degree(int node) const {
    if (!(is_valid_node(node))) throw std::runtime_error("Node does not exist");
    return offsets[node + 1] - offsets[node];
}

bool Graph::has_edge(int source, int target) const {
    // might be optimized by checking whether source's or target's adjacency list is smaller
    for (int i = offsets[source]; i < offsets[source + 1]; i++) {
        if (edges[i] == target) return true;
    }
    return false;
}

bool Graph::is_valid_node(int node) const { return node < num_nodes; }

// Graph: undirected, no loops (A->A)
// Edge file format: sorted, every edge is "descending" (a b -> b < a)
void Graph::read_edges(std::string edges_path) {  // TODO: why so slow?
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

void Graph::read_features(std::string features_path) {  // TODO: why so slow?
    features = std::vector<std::vector<double>>(num_nodes, std::vector<double>(num_features));
    missing = std::vector<std::vector<bool>>(num_nodes, std::vector<bool>(num_features));

    std::ifstream file(features_path);
    if (!file.is_open()) throw std::runtime_error("Could not open file");

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream line_stream(line);

        int node;
        line_stream >> node;  // first value in each row is the node

        // parse features, except last feature and label (because of different formatting)
        for (int i = 0; i < num_features - 2; i++) {
            std::string feature;
            std::getline(line_stream, feature, ',');

            try {
                features[node][i] = std::stod(feature);
            } catch (const std::invalid_argument& e) {  // for missing features
                missing[node][i] = true;
            }
        }

        std::string feature;
        std::getline(line_stream, feature, '\t');
        int last_feature_idx = num_features - 2;
        try {
            features[node][last_feature_idx] = std::stod(feature);
        } catch (const std::invalid_argument& e) {  // for missing features
            missing[node][last_feature_idx] = true;
        }

        std::string label;
        std::getline(line_stream, label);
        int label_idx = num_features - 1;
        try {
            features[node][label_idx] = std::stoi(label);
        } catch (const std::invalid_argument& e) {  // for missing features
            missing[node][label_idx] = true;
        }
    }

    file.close();
}

// Prints unique edges
void Graph::print_edges() const {
    for (int i = 0; i < num_nodes; i++) {
        for (int j = offsets[i]; j < offsets[i + 1]; j++) {
            int neighbour = edges[j];
            if (neighbour <= i) {  // to only print unique edges (in descending order)
                std::cout << i << "\t" << neighbour << "\n";
            }
        }
    }
}

void Graph::print_features() const {
    for (int node = 0; node < num_nodes; node++) {
        std::cout << node << '\t';
        for (int i = 0; i < num_features - 2; i++) {
            if (missing[node][i]) {
                std::cout << "\'#\'" << ", ";
            } else {
                std::cout << features[node][i] << ", ";
            }
        }

        if (missing[node][num_features - 2]) {  // last feature
            std::cout << "\'#\'" << '\t';
        } else {
            std::cout << features[node][num_features - 2] << '\t';
        }

        if (missing[node][num_features - 1]) {  // label
            std::cout << "\'#\'" << '\n';
        } else {
            std::cout << features[node][num_features - 1] << '\n';
        }
    }
}

// Features file format: lines ordered ascending, newline after last line
int parse_node_count(std::string features_path) {
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

// Features file format: "1 0.93, '#', -3.2 2"
int parse_feature_count(std::string features_path) {
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

std::vector<int> remove_duplicates(const std::vector<int>& arr) {
    std::unordered_set<int> seen;
    std::vector<int> result;

    for (const int& num : arr) {
        if (seen.insert(num).second) {  // Only insert if not already in set
            result.push_back(num);
        }
    }

    return result;
}