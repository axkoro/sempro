#pragma once

#include <unordered_map>
#include <vector>

#include "Imputer.hpp"

template <typename T>
class CommunityImputer : public Imputer<T> {
   public:
    CommunityImputer(AttributedGraph<T>& graph, const std::vector<int>& communities)
        : Imputer<T>(graph), communities(communities) {};

    void run();

   private:
    const std::vector<int>& communities;  // Community assignments for each node

    double compute_community_average(int community, int feature);
    static uint64_t encode_comm_feature_pair(int community, int feature);
};

#include "CommunityImputer.tpp"