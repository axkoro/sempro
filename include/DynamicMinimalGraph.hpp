#pragma once
#include <vector>

#include "Graph.hpp"
#include "MinimalGraph.hpp"

class DynamicMinimalGraph : public MinimalGraph {
   private:
    std::vector<std::vector<std::pair<int, int>>> edges;
    int num_edges;

   public:
    DynamicMinimalGraph(int n);
    DynamicMinimalGraph(const Graph& g);

    int get_num_nodes() const override;
    int get_num_edges() const override;

    int get_degree(int u) const override;
    const std::vector<std::pair<int, int>>& get_neighbours(int u) const override;

    // Add an edge with a given weight in an undirected manner
    void add_edge(int u, int v, int w);
};