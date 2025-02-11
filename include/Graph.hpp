#pragma once

#include <stdexcept>
#include <string>
#include <vector>

// forward declarations for friend classes
class WeightedGraph;
class WeightedEdgeIterator;

class GraphException : public std::runtime_error {  // TODO: do we need this?
   public:
    explicit GraphException(const std::string& message) : std::runtime_error(message) {};
};

template <typename T>  // TODO: restrict to numeric types (or something similar)
class Graph {
    friend class WeightedGraph;
    friend class WeightedEdgeIterator;

   protected:
    int num_nodes;
    int num_features;
    std::vector<int> offsets;
    std::vector<int> edges;
    std::vector<int> labels;
    std::vector<std::vector<bool>> missing;
    std::vector<std::vector<T>> features;

   public:
    /**
     * @brief Default constructor. Creates an empty graph.
     */
    Graph() = default;

    /**
     * @brief Constructs a graph by reading edge and feature files.
     *
     * @param edges_path Path to the file containing edges.
     * @param features_path Path to the file containing features.
     */
    Graph(std::string edges_path, std::string features_path);

    /**
     * @brief Debugging/testing constructor for creating a Graph without features.
     */
    Graph(std::vector<int> offsets, std::vector<int> edges);  // TODO: remove

    // Getters

    int get_num_nodes() const;
    int get_num_features() const;
    int get_num_edges() const;

    T get_feature(int node, int feature) const;

    int get_label(int node) const;

    /**
     * Returns a vector of feature indices that are missing for a given node.
     * @param node The node index to check for missing features
     * @return Vector containing indices of all missing features for the node
     */
    std::vector<int> get_missing_features(int node) const;  // TODO: use iterator instead

    /**
     * @brief Gets the immediate neighbours of a node.
     *
     * @param node Index of the node.
     * @return Vector of neighbour node indices.
     * @throws std::logic_error If the node does not exist.
     */
    std::vector<int> get_neighbours(int node) const;  // TODO: use iterator instead

    /**
     * @brief Gets the neighbours of a node up to a certain depth.
     *
     * @param node Index of the node.
     * @param depth Depth of neighbour search.
     * @return Vector of neighbour node indices.
     * @throws std::logic_error If the node does not exist.
     */
    std::vector<int> get_neighbours(int node, int depth) const;  // TODO: ? use iterator instead

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
     * @note Had to be implemented in the subclasses because the abstract Graph class doesn't have
     * the features vector as a member variable.
     */
    void print_features() const;

    /**
     * @brief Prints features using print_features to a file with the given path.
     * @param output_path Path to the file to write the features to.
     */
    void print_features_to_file(std::string output_path) const;

   private:
    void parse_features_line(std::string& line);
};

// Utility functions

/**
 * @brief Parses the number of nodes from the features file.
 *
 * @param features_path Path to the features file.
 * @return Total number of nodes.
 * @throws std::runtime_error If file errors occur or parsing fails.
 */
int parse_node_count(std::string features_path);  // TODO: make static private class members

/**
 * @brief Parses the number of features from the features file.
 *
 * @param features_path Path to the features file.
 * @return Total number of features per node.
 * @throws std::runtime_error If file errors occur or parsing fails.
 */
int parse_feature_count(std::string features_path);

#include "Graph.tpp"