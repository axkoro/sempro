#pragma once

#include "Graph.hpp"

#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test

class WeightedEdgeIterator;

/**
 * @class GraphEdgeWeights
 * @brief Encapsulates per-edge weight data for a Graph.
 *
 * GraphEdgeWeights manages a vector of weights corresponding to the edges of an
 * associated Graph. It does not forward any Graph methods; instead, it solely
 * focuses on maintaining edge weights. Each weight is initialized to a default
 * value of 1.0.
 *
 * An iterator interface (via WeightedEdgeIterator) is provided to traverse the
 * weighted edges. Each iterator returns the current edge (as a (source, target)
 * pair) and a modifiable reference to its associated weight.
 *
 * Example usage:
 * @code
 *   Graph graph("edges.txt", "features.txt");
 *   GraphEdgeWeights edgeWeights(graph);
 *
 *   // Iterate over all edges and update their weights.
 *   for (auto it = edgeWeights.begin(); it != edgeWeights.end(); ++it) {
 *       auto [source, target] = it.get_edge();
 *       *it = compute_weight(source, target);
 *   }
 * @endcode
 */
class GraphEdgeWeights {
    friend class WeightedEdgeIterator;
    FRIEND_TEST(GraphEdgeWeightsTest, DefaultWeights);
    FRIEND_TEST(GraphEdgeWeightsTest, ModifyWeightsViaIterator);

   public:
    struct Edge {
        int target;
        double weight;
    };

    GraphEdgeWeights(Graph& graph);

    WeightedEdgeIterator begin();
    WeightedEdgeIterator end();

    std::vector<Edge> get_edges(int node) const;

   private:
    Graph& graph;
    std::vector<double> edge_weights;
};

class WeightedEdgeIterator {
    friend class GraphEdgeWeights;

    GraphEdgeWeights* weighted_graph;
    size_t index;
    size_t current_node;  // current source node

   public:
    WeightedEdgeIterator(GraphEdgeWeights* wg, size_t idx);

    WeightedEdgeIterator& operator++();
    bool operator!=(const WeightedEdgeIterator& other) const;
    double& operator*();

    // Returns the current edge as a (source, target) pair.
    std::pair<int, int> get_edge() const;
};
