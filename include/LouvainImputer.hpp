#pragma once

#include <unordered_map>
#include <vector>

#include "GraphBool.hpp"
#include "GraphDouble.hpp"
#include "GraphInt.hpp"
#include "Imputer.hpp"

class LouvainImputer : public Imputer {
   public:
    explicit LouvainImputer(GraphBool& graph, const std::vector<int>& communities)
        : Imputer(graph), communities(communities), type(Graph::feature_type::b) {}

    explicit LouvainImputer(GraphDouble& graph, const std::vector<int>& communities)
        : Imputer(graph), communities(communities), type(Graph::feature_type::d) {}

    explicit LouvainImputer(GraphInt& graph, const std::vector<int>& communities)
        : Imputer(graph), communities(communities), type(Graph::feature_type::i) {}

    void run();

   private:
    const std::vector<int> communities;                       // Community assignments for each node
    Graph::feature_type type;                                 // Type of features in the graph
    double compute_community_average(int node, int feature);  // Compute community average
};

uint64_t encode_comm_feature_pair(int community, int feature);