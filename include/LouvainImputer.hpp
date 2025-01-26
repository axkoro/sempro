#ifndef LOUVAIN_IMPUTER_HPP
#define LOUVAIN_IMPUTER_HPP

#include <unordered_map>
#include <vector>

#include "GraphBool.hpp"
#include "GraphDouble.hpp"
#include "GraphInt.hpp"

class LouvainImputer {
   public:
    enum feature_type { b, d, i };  // bool, double, int

    // Constructors
    LouvainImputer(GraphBool& graph, const std::vector<int>& communities);
    LouvainImputer(GraphDouble& graph, const std::vector<int>& communities);
    LouvainImputer(GraphInt& graph, const std::vector<int>& communities);

    // Perform feature imputation
    void run();

   private:
    Graph& graph;                        // Reference to the graph
    const std::vector<int> communities;  // Community assignments for each node
    feature_type type;                   // Type of features in the graph
};

#endif
