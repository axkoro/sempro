#ifndef KNNIMPUTER_HPP
#define KNNIMPUTER_HPP

#include "Graph.hpp"

class KNNImputer {
   private:
    Graph& graph;
    int k;

   public:
    KNNImputer(Graph& graph);

    void run();

    // Configuration
    void set_depth(int k);
};

double compute_global_average(Graph& graph, int feature);

#endif