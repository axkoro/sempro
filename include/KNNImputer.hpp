#ifndef KNNIMPUTER_HPP
#define KNNIMPUTER_HPP

#include "Graph.hpp"

class KNNImputer {
   private:
    Graph& graph;
    int k;

   public:
    KNNImputer(Graph& graph);
    void set_depth(int k);
    void run();
};

#endif