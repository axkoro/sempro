#pragma once

#include <omp.h>

#include <iostream>
#include <numeric>
#include <unordered_map>

#include "CommunityImputer.hpp"

template <typename T>
void CommunityImputer<T>::run() {
    std::unordered_map<uint64_t, double> averages;  // (community, feature) -> average
    int num_nodes = this->graph.get_num_nodes();
    int num_feautures = this->graph.get_num_features();

// Impute missing features with community means
#pragma omp parallel for schedule(dynamic)
    for (int node = 0; node < num_nodes; ++node) {
        int community = communities[node];

        for (int feature = 0; feature < num_feautures; ++feature) {
            if (this->graph.is_missing(node, feature)) {
                double average;
                uint64_t key = encode_comm_feature_pair(community, feature);
                bool found = false;

#pragma omp critical(averages)
                {
                    auto it = averages.find(key);
                    if (it != averages.end()) {
                        average = it->second;
                        found = true;
                    }
                }

                if (!found) {
                    T computed_average =
                        round_value<T>(compute_community_average(community, feature));

#pragma omp critical(averages)
                    {
                        auto it = averages.find(
                            key);  // search again, because another thread might've already inserted
                                   // by the time we computed the average
                        if (it != averages.end()) {
                            average = it->second;
                        } else {
                            averages.emplace(key, computed_average);
                            average = computed_average;
                        }
                    }
                }

#pragma omp critical
                {
                    T rounded_average = round_value<T>(average);
                    this->graph.set_feature(node, feature, rounded_average);

                    this->graph.set_missing(node, feature,
                                            false);  // to use this feature for other imputations
                }
            }
        }
    }
}

template <typename T>
double CommunityImputer<T>::compute_community_average(int community, int feature) {
    double sum = 0.0;
    int count = 0;
    int num_nodes = this->graph.get_num_nodes();

#pragma omp parallel for reduction(+ : sum, count)
    for (int node = 0; node < num_nodes; ++node) {
        bool node_in_community = communities[node] == community;
        bool feature_not_missing = !this->graph.is_missing(node, feature);
        if (node_in_community && feature_not_missing) {
            T feature_val = this->graph.get_feature(node, feature);
            sum += feature_val;
            ++count;
        }
    }

    bool feature_not_in_community = count == 0;
    if (feature_not_in_community) {
        return Imputer<T>::compute_global_average(feature);
    }

    return sum / count;
}

template <typename T>
uint64_t CommunityImputer<T>::encode_comm_feature_pair(int community, int feature) {
    return ((uint64_t)community) << 32 | (uint64_t)feature;
}