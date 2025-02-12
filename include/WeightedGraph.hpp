#pragma once

#include "Graph.hpp"

#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test

class WeightedEdgeIterator;

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
class WeightedGraph {  // TODO: reduce this to just an edge data structure (no function forwarding
                       // anymore)
    friend class WeightedEdgeIterator;
    FRIEND_TEST(WeightedGraphEdgesTest, DefaultWeights);
    FRIEND_TEST(WeightedGraphEdgesTest, ModifyWeightsViaIterator);

   public:
    struct Edge {
        int target;
        double weight;
    };

    WeightedGraph(Graph& graph);

    WeightedEdgeIterator begin();
    WeightedEdgeIterator end();

    std::vector<Edge> get_edges(int node) const;
    std::vector<Edge> get_edges(int node, int depth) const;

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