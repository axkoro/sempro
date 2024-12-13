#pragma once

#include "Graph.hpp"
#include "Imputer.hpp"

class KNNImputer : public Imputer {
   public:
    static constexpr int DEFAULT_K = 3;

   private:
    int k = DEFAULT_K;

   public:
    explicit KNNImputer(Graph& g);

    void run();

    // Configuration
    void set_depth(int k);
};

double compute_global_average(Graph& graph, int feature);