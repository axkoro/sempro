#ifndef LOUVAIN_IMPUTER_HPP
#define LOUVAIN_IMPUTER_HPP

#include "Graph.hpp"
#include <vector>
#include <unordered_map>

class LouvainImputer {
public:
    // Constructor
    LouvainImputer(Graph& graph, const std::vector<int>& communities);

    // Perform feature imputation
    void run();

    // Helper to compute mean feature values for each community
    std::unordered_map<int, std::vector<double>> compute_community_average();


private:
    Graph& graph; // Reference to the graph
    const std::vector<int> communities; // Community assignments for each node

};

#endif 
