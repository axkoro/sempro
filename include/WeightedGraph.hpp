#pragma once

#include "Graph.hpp"
#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test

class WeightedEdgeIterator;  // Forward declaration

/**
 * @class WeightedGraph
 * @brief A lightweight wrapper that decorates a Graph with per-edge weights.
 *
 * WeightedGraph wraps an underlying Graph instance and augments it with a vector
 * of edge weights. All graph-related method calls (e.g. neighbor queries, feature
 * accesses) are forwarded to the underlying Graph. The edge weights vector is
 * initialized to the same size as the Graph's edge list, with every edge given a
 * default weight of 1.0.
 *
 * In addition, WeightedGraph provides an iterator interface (via WeightedEdgeIterator)
 * that allows clients to iterate over the edges. Each iterator instance gives access
 * to both the current edge (as a (source, target) pair) and a modifiable reference
 * to its corresponding weight.
 *
 * Example usage:
 * @code
 *   Graph graph("edges.txt", "features.txt");
 *   WeightedGraph wgraph(graph);
 *
 *   // Iterate over all edges and update their weights.
 *   for (auto it = wgraph.begin(); it != wgraph.end(); ++it) {
 *       auto [source, target] = it.get_edge();
 *       *it = compute_weight(source, target);
 *   }
 * @endcode
 */
class WeightedGraph {
    friend class WeightedEdgeIterator;
    FRIEND_TEST(WeightedGraphEdgesTest, DefaultWeights);
    FRIEND_TEST(WeightedGraphEdgesTest, ModifyWeightsViaIterator);

   public:
    struct Edge {
        int target;
        double weight;
    };

    WeightedGraph(Graph& graph);

    // Iterator support
    WeightedEdgeIterator begin();
    WeightedEdgeIterator end();

    // Edge functionality
    std::vector<Edge> get_edges(int node) const;
    std::vector<Edge> get_edges(int node, int depth) const;

    // Forwarding functions: these simply call the corresponding function on 'graph'
    int get_num_nodes() const;
    int get_num_features() const;
    int get_num_edges() const;

    bool get_bool_feature(int node, int feature) const;
    double get_double_feature(int node, int feature) const;
    int get_int_feature(int node, int feature) const;

    int get_label(int node) const;
    std::vector<int> get_missing_features(int node) const;
    int get_degree(int node) const;

    // Setters
    virtual void set_bool_feature(int node, int feature, bool value);
    virtual void set_double_feature(int node, int feature, double value);
    virtual void set_int_feature(int node, int feature, int value);
    void set_missing(int node, int feature, bool value);

    // Queries
    bool has_edge(int source, int target) const;
    bool is_missing(int node, int feature) const;
    bool is_valid_node(int node) const;

   private:
    Graph& graph;
    std::vector<double> edge_weights;
};

class WeightedEdgeIterator {
    friend class WeightedGraph;

    WeightedGraph* weighted_graph;
    size_t index;
    size_t current_node;  // current source node

   public:
    WeightedEdgeIterator(WeightedGraph* wg, size_t idx);

    WeightedEdgeIterator& operator++();
    bool operator!=(const WeightedEdgeIterator& other) const;
    double& operator*();

    // Returns the current edge as a (source, target) pair.
    std::pair<int, int> get_edge() const;
};