#pragma once

#include <vector>

#include "Graph.hpp"

class DynamicGraph {
   private:
    std::vector<std::vector<std::pair<int, int>>> edges;
    int num_edges;

   public:
    DynamicGraph(int n);
    DynamicGraph(const Graph& g);

    int get_num_nodes() const;
    int get_num_edges() const;

    int get_degree(int u) const;
    const std::vector<std::pair<int, int>>& get_neighbors(int u) const;

    // Add an edge with a given weight in an undirected manner
    void add_edge(int u, int v, int w);
};