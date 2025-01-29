#pragma once

#include "MinimalGraph.hpp"

class StaticMinimalGraph : public MinimalGraph {
   private:
    std::vector<int> offsets;
    std::vector<std::pair<int, int>> edges;

   public:
    StaticMinimalGraph::StaticMinimalGraph(std::vector<int> offsets,
                                           std::vector<std::pair<int, int>> edges)
        : offsets(offsets), edges(edges) {};

    virtual int get_num_nodes() const override;
    virtual int get_num_edges() const override;

    virtual int get_degree(int u) const override;
    virtual const std::vector<std::pair<int, int>>& get_neighbours(int u) const override;
};