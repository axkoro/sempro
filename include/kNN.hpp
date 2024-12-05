#include "Graph.hpp"

class kNN {
   public:
    kNN(Graph& graph, int k);
    void run();

   private:
    Graph& graph;
    int k;
};