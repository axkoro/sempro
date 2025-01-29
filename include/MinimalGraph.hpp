#pragma once

#include <vector>

/**
 * @class MinimalGraph
 * @brief A representation of a weighted graph, but without node attributes.
 *
 * This class provides an abstract interface for a graph structure. It is minimal in the sense that
 * it does not store any feature information about the nodes, only the basic structure of the graph.
 */
class MinimalGraph {
   public:
    // MinimalGraph(int n);
    // MinimalGraph(const Graph& g);

    virtual int get_num_nodes() const = 0;
    virtual int get_num_edges() const = 0;

    // Return sum of weights of edges from node "u"  (the "weighted degree")
    virtual int get_degree(int u) const = 0;
};