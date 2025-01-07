#pragma once

#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

class GraphException : public std::runtime_error {
   public:
    explicit GraphException(const std::string& message);
};

template <typename T>
class Graph {
   private:
    int num_nodes = -1;
    int num_features = -1;
    std::vector<int> offsets;
    std::vector<int> edges;
    std::vector<int> labels;
    std::vector<std::vector<T>> features;
    std::vector<std::vector<bool>> missing;

   public:
    // Constructors

    /**
     * @brief Default constructor. Creates an empty graph.
     */
    Graph();

    /**
     * @brief Constructs a graph from sparse matrix representation.
     *
     * @param offsets Array containing offsets into edges array for each node
     * @param edges Array containing target node IDs for edges
     */
    Graph(std::vector<int>& offsets, std::vector<int>& edges);  // for testing

    /**
     * @brief Constructs a graph by reading edge and feature files.
     *
     * @param edges_path Path to the file containing edges.
     * @param features_path Path to the file containing features.
     */
    Graph(std::string edges_path, std::string features_path);

    // Getters

    int get_num_nodes() const;
    int get_num_features() const;

    T get_feature(int node, int feature) const;

    /**
     * Returns a vector of feature indices that are missing for a given node.
     * @param node The node index to check for missing features
     * @return Vector containing indices of all missing features for the node
     */
    std::vector<int> get_missing_features(int node) const;

    /**
     * @brief Gets the immediate neighbours of a node.
     *
     * @param node Index of the node.
     * @return Vector of neighbour node indices.
     * @throws std::logic_error If the node does not exist.
     */
    std::vector<int> get_neighbours(int node) const;

    /**
     * @brief Gets the neighbours of a node up to a certain depth.
     *
     * @param node Index of the node.
     * @param depth Depth of neighbour search.
     * @return Vector of neighbour node indices.
     * @throws std::logic_error If the node does not exist.
     */
    std::vector<int> get_neighbours(int node, int depth) const;

    /**
     * @brief Gets the degree of a node.
     *
     * @param node Index of the node.
     * @return Degree of the node.
     * @throws std::logic_error If the node does not exist.
     */
    int get_degree(int node) const;

    // Setters

    void set_feature(int node, int feature, T value);
    void set_missing(int node, int feature, bool value);

    // Queries

    /**
     * @brief Checks if an edge exists between two nodes.
     *
     * @param source Index of the source node.
     * @param target Index of the target node.
     * @return True if the edge exists, false otherwise.
     */
    bool has_edge(int source, int target) const;

    /**
     * @brief Checks if a given feature is missing for a node.
     *
     * @param node Index of the node.
     * @param feature Index of the feature.
     * @return True if the feature is missing, false otherwise.
     */
    bool is_missing(int node, int feature) const;

    /**
     * @brief Validates if a node index is within the graph.
     *
     * @param node Index of the node.
     * @return True if the node is valid, false otherwise.
     */
    bool is_valid_node(int node) const;

    // File I/O

    /**
     * @brief Reads edges from a file and builds the graph.
     *
     * @param edges_path Path to the file containing edges.
     * @throws std::runtime_error If the number of nodes is uninitialized or file errors occur.
     */
    void read_edges(std::string edges_path);

    /**
     * @brief Reads features from a file and initializes feature data.
     *
     * @param features_path Path to the file containing features.
     * @throws std::runtime_error If file errors occur or parsing fails.
     */
    void read_features(std::string features_path);

    // Debug/Display

    /**
     * @brief Prints unique edges of the graph to the console.
     */
    void print_edges() const;

    /**
     * @brief Prints features of all nodes to the console.
     */
    void print_features() const;
};

// Utility functions

/**
 * @brief Parses the number of nodes from the features file.
 *
 * @param features_path Path to the features file.
 * @return Total number of nodes.
 * @throws std::runtime_error If file errors occur or parsing fails.
 */
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

/**
 * @brief Parses the number of features from the features file (excluding the label).
 *
 * @param features_path Path to the features file.
 * @return Total number of features per node.
 * @throws std::runtime_error If file errors occur or parsing fails.
 */
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

/**
 * @brief Removes duplicate integers from a vector.
 *
 * @param arr Input vector.
 * @return Vector with duplicates removed.
 */
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

// Implementations (type-independent)

template <typename T>
Graph<T>::Graph() = default;

template <typename T>
Graph<T>::Graph(std::vector<int>& offsets, std::vector<int>& edges) {
    this->offsets = offsets;
    this->edges = edges;
    num_nodes = offsets.size() - 1;
}

template <typename T>
Graph<T>::Graph(std::string edges_path, std::string features_path) {
    num_nodes = parse_node_count(features_path);
    num_features = parse_feature_count(features_path);
    read_edges(edges_path);
    read_features(features_path);
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
T Graph<T>::get_feature(int node, int feature) const {
    return features[node][feature];
}

template <typename T>
void Graph<T>::set_feature(int node, int feature, T value) {
    features[node][feature] = value;
}

template <typename T>
void Graph<T>::set_missing(int node, int feature, bool value) {
    missing[node][feature] = value;
}

template <typename T>
std::vector<int> Graph<T>::get_missing_features(int node) const {
    std::vector<int> missing_features_list;
    for (int feature = 0; feature < num_features; feature++) {
        if (is_missing(node, feature)) missing_features_list.push_back(feature);
    }

    return missing_features_list;
}

template <typename T>
std::vector<int> Graph<T>::get_neighbours(int node) const {
    if (!(is_valid_node(node))) throw std::logic_error("Node does not exist");

    std::vector<int> neighbours(offsets[node + 1] - offsets[node]);
    std::copy(edges.begin() + offsets[node], edges.begin() + offsets[node + 1], neighbours.begin());

    return neighbours;
}

template <typename T>
std::vector<int> Graph<T>::get_neighbours(int node, int depth) const {
    if (!(is_valid_node(node))) throw std::logic_error("Node does not exist");

    std::vector<int> neighbours = get_neighbours(node);  // k=1

    int last_depth_start = 0;
    int this_depth_start = neighbours.size();
    for (int k = 2; k <= depth; k++) {
        for (int i = last_depth_start; i < this_depth_start; i++) {
            std::vector<int> new_neighbours = get_neighbours(neighbours[i]);
            neighbours.insert(neighbours.end(), new_neighbours.begin(), new_neighbours.end());
        }
        neighbours = remove_duplicates(neighbours);
        last_depth_start = this_depth_start;
        this_depth_start = neighbours.size();
    }

    return neighbours;
}

template <typename T>
int Graph<T>::get_degree(int node) const {
    if (!(is_valid_node(node))) throw std::logic_error("Node does not exist");
    return offsets[node + 1] - offsets[node];
}

template <typename T>
bool Graph<T>::has_edge(int source, int target) const {
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
    return missing[node][feature];
}

template <typename T>
void Graph<T>::read_edges(std::string edges_path) {
    if (num_nodes == -1) {
        throw std::runtime_error("num_nodes needs to be initialised before calling read_edges");
    }

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
    std::cout << std::noboolalpha;  // bools will be printed as 1/0 instead of true/false

    for (int node = 0; node < num_nodes; node++) {
        std::cout << node << '\t';
        for (int i = 0; i < num_features - 1; i++) {
            if (missing[node][i]) {
                std::cout << "\'#\'" << ", ";
            } else {
                std::cout << features[node][i] << ", ";
            }
        }

        if (missing[node][num_features - 1]) {  // last feature
            std::cout << "\'#\'" << '\t';
        } else {
            std::cout << features[node][num_features - 1] << '\t';
        }

        std::cout << labels[node] << '\n';  // label
    }
}

// Implementations (type-dependent)

template <>
void Graph<bool>::read_features(std::string features_path) {
    if (num_nodes == -1) {
        num_nodes = parse_node_count(features_path);
        if (num_nodes < 0)
            throw std::runtime_error("Couldn't parse node count from file: " + features_path);
    }
    if (num_features == -1) {
        num_features = parse_feature_count(features_path);
        if (num_nodes < 0)
            throw std::runtime_error("Couldn't parse feature count from file: " + features_path);
    }

    labels = std::vector<int>(num_nodes);
    features = std::vector<std::vector<bool>>(num_nodes, std::vector<bool>(num_features));
    missing = std::vector<std::vector<bool>>(num_nodes, std::vector<bool>(num_features));

    std::ifstream file(features_path);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + features_path);

    std::string line;
    while (std::getline(file, line)) {
        const char* ptr = line.c_str();
        char* endptr;

        // Parse node index
        int node = std::strtol(ptr, &endptr, 10);
        ptr = endptr;

        // Parse features
        for (int i = 0; i < num_features; i++) {
            u_long value = std::strtoul(ptr, &endptr, 10);
            if (ptr == endptr) {  // found '#'
                missing[node][i] = true;
                endptr += 3;              // advance three characters
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

        // Parse label
        --ptr;  // because last feature and label are only separated by '\t'
        labels[node] = std::strtol(ptr, NULL, 10);
    }
    file.close();
}

template <>
void Graph<int>::read_features(std::string features_path) {
    if (num_nodes == -1) {
        num_nodes = parse_node_count(features_path);
        if (num_nodes < 0)
            throw std::runtime_error("Couldn't parse node count from file: " + features_path);
    }
    if (num_features == -1) {
        num_features = parse_feature_count(features_path);
        if (num_nodes < 0)
            throw std::runtime_error("Couldn't parse feature count from file: " + features_path);
    }

    labels = std::vector<int>(num_nodes);
    features = std::vector<std::vector<int>>(num_nodes, std::vector<int>(num_features));
    missing = std::vector<std::vector<bool>>(num_nodes, std::vector<bool>(num_features));

    std::ifstream file(features_path);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + features_path);

    std::string line;
    while (std::getline(file, line)) {
        const char* ptr = line.c_str();
        char* endptr;

        // Parse node index
        int node = std::strtol(ptr, &endptr, 10);
        ptr = endptr;

        // Parse features
        for (int i = 0; i < num_features; i++) {
            long value = std::strtol(ptr, &endptr, 10);
            if (ptr == endptr) {  // found '#'
                missing[node][i] = true;
                endptr += 3;              // advance three characters
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

        // Parse label
        --ptr;  // because last feature and label are only separated by '\t'
        labels[node] = std::strtol(ptr, NULL, 10);
    }
    file.close();
}

template <>
void Graph<double>::read_features(std::string features_path) {
    if (num_nodes == -1) {
        num_nodes = parse_node_count(features_path);
        if (num_nodes < 0)
            throw std::runtime_error("Couldn't parse node count from file: " + features_path);
    }
    if (num_features == -1) {
        num_features = parse_feature_count(features_path);
        if (num_nodes < 0)
            throw std::runtime_error("Couldn't parse feature count from file: " + features_path);
    }

    labels = std::vector<int>(num_nodes);
    features = std::vector<std::vector<double>>(num_nodes, std::vector<double>(num_features));
    missing = std::vector<std::vector<bool>>(num_nodes, std::vector<bool>(num_features));

    std::ifstream file(features_path);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + features_path);

    std::string line;
    while (std::getline(file, line)) {
        const char* ptr = line.c_str();
        char* endptr;

        // Parse node index
        int node = std::strtol(ptr, &endptr, 10);
        ptr = endptr;

        // Parse features
        for (int i = 0; i < num_features; i++) {
            double value = std::strtod(ptr, &endptr);
            if (ptr == endptr) {  // found '#'
                missing[node][i] = true;
                endptr += 3;  // advance three characters
            } else {
                features[node][i] = value;
            }
            ptr = endptr + 2;  // skip comma and space
        }

        // Parse label
        --ptr;  // because last feature and label are only separated by '\t'
        int label = std::strtol(ptr, &endptr, 10);
        labels[node] = label;
    }
    file.close();
}