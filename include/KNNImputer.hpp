#include "Graph.hpp"

class KNNImputer {
   public:
    KNNImputer(Graph& graph, int k);
    void run();

   private:
    Graph& graph;
    int k;
};