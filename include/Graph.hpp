#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <stdexcept>
#include <string>
#include <vector>

class GraphException : public std::runtime_error {
   public:
    explicit GraphException(const std::string& message);
};

// Forward declarations for friend classes
class KNNImputer;
class ClusterImputer;
class GCNImputer;

class Graph {
    friend class KNNImputer;
    friend class ClusterImputer;
    friend class GCNImputer;

   private:
    int num_nodes = -1;
    int num_features = -1;
    std::vector<int> offsets;
    std::vector<int> edges;
    std::vector<std::vector<double>> features;
    std::vector<std::vector<bool>> missing;

   public:
    // Constructors
    Graph();
    Graph(std::vector<int>& offsets, std::vector<int>& edges);  // for testing
    Graph(std::string edges_path, std::string features_path);

    // Getters
    int get_num_nodes() const;
    int get_num_features() const;
    std::vector<double> get_features(int node) const;
    std::vector<bool> get_missing_features(int node) const;
    std::vector<int> get_neighbours(int node) const;
    std::vector<int> get_neighbours(int node, int depth) const;
    int get_degree(int node) const;
    std::vector<int> get_offsets() const;
    std::vector<int> get_edges() const;

    // Queries
    bool has_edge(int source, int target) const;
    bool is_valid_node(int node) const;

    // File I/O
    void read_edges(std::string edges_path);
    void read_features(std::string features_path);

    // Debug/Display
    void print_edges() const;
    void print_features() const;
};

// Utility functions
int parse_node_count(std::string features_path);
int parse_feature_count(std::string features_path);
std::vector<int> remove_duplicates(const std::vector<int>& arr);

#endif