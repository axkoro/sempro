#include "Graph.hpp"

class kNN {
   public:
    kNN(Graph& graph, int k);
    void runkNN();

   private:
    Graph& graph;
    int k;
};