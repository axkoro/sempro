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

double compute_global_average_bool(Graph& graph, int feature);
double compute_global_average_double(Graph& graph, int feature);
double compute_global_average_int(Graph& graph, int feature);

bool to_bool(double value);
int to_int(double value);