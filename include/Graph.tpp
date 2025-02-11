#pragma once

#include <algorithm>
#include <charconv>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <type_traits>
#include <unordered_set>

#include "Graph.hpp"

template <typename T>
Graph<T>::Graph(std::string edges_path, std::string features_path) {
    if constexpr (!std::is_arithmetic_v<T>) {
        throw std::domain_error("Graph class does not support non-arithmetic types");
    }

    num_nodes = parse_node_count(features_path);
    num_features = parse_feature_count(features_path);
    read_edges(edges_path);
    read_features(features_path);
}

template <typename T>
Graph<T>::Graph(std::vector<int> offsets, std::vector<int> edges) {
    this->offsets = offsets;
    this->edges = edges;
    num_nodes = offsets.size() - 1;
}

template <typename T>
int Graph<T>::get_num_nodes() const {
    return num_nodes;
}

template <typename T>
int Graph<T>::get_num_features() const {
    return num_features;
}

template <typename T>
int Graph<T>::get_num_edges() const {
    return edges.size() / 2;
}

template <typename T>
T Graph<T>::get_feature(int node, int feature) const {
    if (!(is_valid_node(node))) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");
    return features[node][feature];
}

template <typename T>
int Graph<T>::get_label(int node) const {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    return labels[node];
}

template <typename T>
void Graph<T>::set_feature(int node, int feature, T value) {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");
    features[node][feature] = value;
}

template <typename T>
void Graph<T>::set_missing(int node, int feature, bool value) {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");
    missing[node][feature] = value;
}

template <typename T>
std::vector<int> Graph<T>::get_missing_features(int node) const {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    std::vector<int> missing_features_list;
    for (int feature = 0; feature < num_features; feature++) {
        if (is_missing(node, feature)) missing_features_list.push_back(feature);
    }

    return missing_features_list;
}

template <typename T>
std::vector<int> Graph<T>::get_neighbours(int node) const {
    if (!is_valid_node(node)) throw GraphException("Node does not exist");

    std::vector<int> neighbours(offsets[node + 1] - offsets[node]);
    std::copy(edges.begin() + offsets[node], edges.begin() + offsets[node + 1], neighbours.begin());

    return neighbours;
}

template <typename T>
std::vector<int> Graph<T>::get_neighbours(int node, int depth) const {
    if (!(is_valid_node(node))) throw GraphException("Node does not exist");

    std::unordered_set<int> visited;
    std::queue<int> frontier;
    frontier.push(node);
    visited.insert(node);

    for (int d = 0; d < depth; ++d) {
        int frontierSize = frontier.size();
        for (int i = 0; i < frontierSize; ++i) {
            int curr = frontier.front();
            frontier.pop();
            for (auto&& nbr : get_neighbours(curr)) {
                if (!visited.count(nbr)) {
                    visited.insert(nbr);
                    frontier.push(nbr);
                }
            }
        }
    }
    std::vector<int> neighbours(visited.begin(), visited.end());

    return neighbours;
}

template <typename T>
int Graph<T>::get_degree(int node) const {
    if (!(is_valid_node(node))) throw GraphException("Node does not exist");
    return offsets[node + 1] - offsets[node];
}

template <typename T>
bool Graph<T>::has_edge(int source, int target) const {
    if (!is_valid_node(source) || !is_valid_node(target))
        throw GraphException("Invalid node index.");
    // might be optimized by checking whether source's or target's adjacency list is smaller
    for (int i = offsets[source]; i < offsets[source + 1]; i++) {
        if (edges[i] == target) return true;
    }
    return false;
}

template <typename T>
bool Graph<T>::is_valid_node(int node) const {
    return node >= 0 && node < num_nodes;
}

template <typename T>
bool Graph<T>::is_missing(int node, int feature) const {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");
    return missing[node][feature];
}

template <typename T>
void Graph<T>::read_edges(std::string edges_path) {
    std::ifstream file(edges_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + edges_path);
    }

    std::vector<std::vector<int>> temp_edges(num_nodes);  // temporary adjecency list

    int a, b;
    while (file >> a >> b) {
        if (!is_valid_node(a) || !is_valid_node(b)) {
            throw std::out_of_range("Node index out of range");
        }
        temp_edges[a].push_back(b);
        temp_edges[b].push_back(a);
    }

    if (file.bad()) {
        throw std::runtime_error("Error reading file: " + edges_path);
    }

    // Flatten to CSR
    offsets = std::vector<int>(num_nodes + 1);
    for (int i = 1; i < offsets.size(); i++) {
        offsets[i] = offsets[i - 1] + temp_edges[i - 1].size();
    }

    int num_edges = offsets[offsets.size() - 1];
    edges = std::vector<int>(num_edges);
    for (int i = 0; i < temp_edges.size(); i++) {
        std::copy(temp_edges[i].begin(), temp_edges[i].end(), edges.begin() + offsets[i]);
    }

    file.close();
}

template <typename T>
void Graph<T>::read_features(std::string features_path) {
    labels = std::vector<int>(num_nodes);
    features = std::vector<std::vector<T>>(num_nodes, std::vector<T>(num_features));
    missing = std::vector<std::vector<bool>>(num_nodes, std::vector<bool>(num_features));

    std::ifstream file(features_path);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + features_path);

    std::string line;
    while (std::getline(file, line)) {
        parse_features_line(line);
    }
    file.close();
}

template <typename T>
void Graph<T>::print_edges() const {
    for (int i = 0; i < num_nodes; i++) {
        for (int j = offsets[i]; j < offsets[i + 1]; j++) {
            int neighbour = edges[j];
            if (neighbour <= i) {  // to only print unique edges (in descending order)
                std::cout << i << "\t" << neighbour << "\n";
            }
        }
    }
}

template <typename T>
void Graph<T>::print_features() const {
    std::cout << std::noboolalpha;  // print 1/0 instead of true/false

    for (int node = 0; node < num_nodes; node++) {
        std::cout << node << '\t';
        for (int i = 0; i < num_features - 1; i++) {
            if (missing[node][i]) {
                std::cout << "#" << ", ";
            } else {
                std::cout << features[node][i] << ", ";
            }
        }

        if (missing[node][num_features - 1]) {  // last feature
            std::cout << "#" << '\t';
        } else {
            std::cout << features[node][num_features - 1] << '\t';
        }

        std::cout << labels[node] << '\n';  // label
    }
}

template <typename T>
void Graph<T>::print_features_to_file(std::string output_path) const {
    std::ofstream file(output_path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + output_path);

    std::streambuf* coutbuf = std::cout.rdbuf();  // save old buf
    std::cout.rdbuf(file.rdbuf());                // redirect std::cout to file

    print_features();

    std::cout.rdbuf(coutbuf);  // reset to standard output again
    file.close();
}

template <typename T>
void Graph<T>::parse_features_line(std::string& line) {
    const char* ptr = line.c_str();
    char* endptr;

    // Parse node index
    int node = std::strtol(ptr, &endptr, 10);
    ptr = endptr;

    // Parse features
    if constexpr (std::is_integral_v<T>) {
        for (int i = 0; i < num_features; i++) {
            long value = std::strtol(ptr, &endptr, 10);
            if (ptr == endptr) {  // found #
                missing[node][i] = true;
                endptr += 1;              // skip the #
            } else if (*endptr == '.') {  // found value with comma
                features[node][i] = value;

                // skip decimal places
                if (i != num_features - 1) {
                    endptr += strcspn(endptr, ",");
                } else {
                    endptr += strcspn(endptr, "\t");
                }
            } else {  // found value without comma
                features[node][i] = value;
            }
            ptr = endptr + 2;  // skip comma and space
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        for (int i = 0; i < num_features; i++) {
            double value = std::strtod(ptr, &endptr);
            if (ptr == endptr) {  // found #
                missing[node][i] = true;
                endptr += 1;  // skip the #
            } else {
                features[node][i] = value;
            }
            ptr = endptr + 2;  // skip comma and space
        }
    } else {
        throw std::domain_error("Can't parse non-numeric types");
    }

    // Parse label
    --ptr;  // because last feature and label are only separated by '\t'
    labels[node] = std::strtol(ptr, NULL, 10);
}

int parse_node_count(std::string features_path) {
    std::ifstream file(features_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + features_path);
    }

    // iterate backwards to find beginning of last line
    file.seekg(-2, file.end);  // -2 because -1 would be '\n' (see assumptions)
    size_t pos = file.tellg();
    while (file.get() != '\n') {
        file.seekg(--pos);
    }

    // get node from last line
    std::string last_line;
    if (!std::getline(file, last_line)) {
        throw std::runtime_error("Failed to read the last line");
    }

    std::istringstream line_stream(last_line);
    int node;
    if (!(line_stream >> node)) {
        throw std::runtime_error("Failed to parse integer");
    }

    file.close();

    if (node < 0) {
        throw std::runtime_error("Invalid node value");
    }

    return node + 1;  // assuming naming starts at 0
}

int parse_feature_count(std::string features_path) {
    std::ifstream file(features_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + features_path);
    }

    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Failed to read the first line");
    }

    int comma_count = std::count(line.begin(), line.end(), ',');
    if (comma_count == 0)
        throw std::runtime_error("Invalid format: no commas found in the line of file '" +
                                 features_path + "'");
    return comma_count + 1;  // assuming one comma after every feature except the last
}