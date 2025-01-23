#pragma once
#include <vector>

#include "Graph.hpp"

class MinimalGraph {
   private:
    std::vector<std::vector<std::pair<int, int>>> edges;
    int num_edges;

   public:
    MinimalGraph(int n);

    MinimalGraph(const Graph& g);

    int get_num_nodes() const;

    int get_num_edges() const;

    // Return adjacency list for node "u"
    // Each entry is (neighbor, weight)
    const std::vector<std::pair<int, int>>& get_neighbours(int u) const;

    // Return sum of weights of edges from node "u"  (the "weighted degree")
    int get_degree(int u) const;

    // Add an edge with a given weight in an undirected manner
    void add_edge(int u, int v, int w);
};