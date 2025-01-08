#pragma once

#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class GraphException : public std::runtime_error {
   public:
    explicit GraphException(const std::string& message);
};

class Graph {
   protected:
    int num_nodes = -1;
    int num_features = -1;
    std::vector<int> offsets;
    std::vector<int> edges;
    std::vector<int> labels;
    std::vector<std::vector<bool>> missing;
    // std::vector<std::vector<Feature>> features;

   public:
    using Feature = std::variant<bool, int, double>;

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

    virtual Feature get_feature(int node, int feature) const = 0;

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

    virtual void set_feature(int node, int feature, Feature value) = 0;
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
    virtual void read_features(std::string features_path) = 0;

    // Debug/Display

    /**
     * @brief Prints unique edges of the graph to the console.
     */
    void print_edges() const;

    /**
     * @brief Prints features of all nodes to the console.
     */
    virtual void print_features() const = 0;
};

// Utility Functions

int parse_node_count(std::string features_path);

int parse_feature_count(std::string features_path);

std::vector<int> remove_duplicates(const std::vector<int>& arr);