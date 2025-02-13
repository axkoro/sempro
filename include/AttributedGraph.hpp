#pragma once

#include "Graph.hpp"

template <typename T>  // TODO: restrict to numeric types (or something similar)
class AttributedGraph : public Graph {
   protected:
    int num_features;
    std::vector<std::vector<bool>> missing;
    std::vector<std::vector<T>> features;
    std::vector<int> labels;

   public:
    /**
     * @brief Default constructor. Creates an empty graph.
     */
    AttributedGraph() = default;

    /**
     * @brief Constructs a graph by reading edge and feature files.
     *
     * @param edges_path Path to the file containing edges.
     * @param features_path Path to the file containing features.
     */
    AttributedGraph(std::string edges_path, std::string features_path);

    // Getters

    int get_num_features() const;
    T get_feature(int node, int feature) const;
    int get_label(int node) const;

    /**
     * Returns a vector of feature indices that are missing for a given node.
     * @param node The node index to check for missing features
     * @return Vector containing indices of all missing features for the node
     */
    std::vector<int> get_missing_features(int node) const;  // TODO: use iterator instead

    // Setters

    void set_feature(int node, int feature, T value);

    void set_missing(int node, int feature, bool value);

    // Queries

    /**
     * @brief Checks if a given feature is missing for a node.
     *
     * @param node Index of the node.
     * @param feature Index of the feature.
     * @return True if the feature is missing, false otherwise.
     */
    bool is_missing(int node, int feature) const;

    // File I/O

    /**
     * @brief Reads features from a file and initializes feature data.
     *
     * @param features_path Path to the file containing features.
     * @throws std::runtime_error If file errors occur or parsing fails.
     */
    void read_features(std::string features_path);

    // Debug/Display

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
    /**
     * @brief Parses the number of nodes from the features file.
     *
     * @param features_path Path to the features file.
     * @return Total number of nodes.
     * @throws std::runtime_error If file errors occur or parsing fails.
     */
    static int parse_node_count_from_feature_file(std::string features_path);

    static int parse_feature_count(std::string features_path);

    void parse_features_line(std::string& line);
};

#include "AttributedGraph.tpp"