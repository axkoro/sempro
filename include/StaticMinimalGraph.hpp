#pragma once

#include "MinimalGraph.hpp"

struct Edge {
    int target;
    double weight;
};

class StaticMinimalGraph : public MinimalGraph {
   private:
    std::vector<int> offsets;
    std::vector<Edge> edges;

   public:
    StaticMinimalGraph(const std::vector<int>& offsets, const std::vector<Edge>& edges)
        : offsets(offsets), edges(edges) {};

    virtual int get_num_nodes() const override;
    virtual int get_num_edges() const override;

    virtual int get_degree(int u) const override;
    const std::vector<Edge>& get_neighbours(int u) const;
};