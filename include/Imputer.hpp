#pragma once

#include "Graph.hpp"

class Imputer {
   protected:
    Graph& graph;

   public:
    explicit Imputer(Graph& g) : graph(g) {}

    virtual void run() = 0;

    virtual ~Imputer() = default;
};