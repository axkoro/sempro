#pragma once

#include <unordered_map>
#include <vector>

#include "GraphBool.hpp"
#include "GraphDouble.hpp"
#include "GraphInt.hpp"
#include "Imputer.hpp"

class LouvainImputer : public Imputer {
   private:
    enum feature_type { b, d, i };  // bool, double, int

   public:
    explicit LouvainImputer(GraphBool& graph, const std::vector<int>& communities)
        : Imputer(graph), communities(communities), type(b) {}

    explicit LouvainImputer(GraphDouble& graph, const std::vector<int>& communities)
        : Imputer(graph), communities(communities), type(d) {}

    explicit LouvainImputer(GraphInt& graph, const std::vector<int>& communities)
        : Imputer(graph), communities(communities), type(i) {}

    void run();

   private:
    const std::vector<int> communities;                       // Community assignments for each node
    feature_type type;                                        // Type of features in the graph
    double compute_community_average(int node, int feature);  // Compute community average
};

uint64_t encode_comm_feature_pair(int community, int feature);