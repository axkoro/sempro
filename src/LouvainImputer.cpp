#include "LouvainImputer.hpp"

#include <omp.h>

#include <iostream>
#include <numeric>
#include <unordered_map>

void LouvainImputer::run() {
    std::unordered_map<uint64_t, double> averages;  // (community, feature) -> average

// Impute missing features with community means
#pragma omp parallel for schedule(dynamic)
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        int community = communities[node];

        for (int feature = 0; feature < graph.get_num_features(); ++feature) {
            if (graph.is_missing(node, feature)) {
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
                    double computedAverage = compute_community_average(community, feature);

#pragma omp critical(averages)
                    {
                        auto it = averages.find(key);  // another thread might've already inserted
                        if (it != averages.end()) {
                            average = it->second;
                        } else {
                            averages.emplace(key, computedAverage);
                            average = computedAverage;
                        }
                    }
                }

#pragma omp critical
                {
                    if (type == b) {
                        graph.set_bool_feature(node, feature, to_bool(average));
                    } else if (type == d) {
                        graph.set_double_feature(node, feature, average);
                    } else if (type == i) {
                        graph.set_int_feature(node, feature, to_int(average));
                    }

                    graph.set_missing(node, feature,
                                      false);  // to use this feature for other imputations
                }
            }
        }
    }
}

double LouvainImputer::compute_community_average(int community, int feature) {
    double sum = 0.0;
    int count = 0;

#pragma omp parallel for reduction(+ : sum, count)
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        if (communities[node] == community && !graph.is_missing(node, feature)) {
            if (type == b) {
                sum += graph.get_bool_feature(node, feature);
            } else if (type == d) {
                sum += graph.get_double_feature(node, feature);
            } else if (type == i) {
                sum += graph.get_int_feature(node, feature);
            }
            ++count;
        }
    }

    bool feature_not_community = count == 0;
    if (feature_not_community) {
        if (type == b) {
            return compute_global_average_bool(graph, feature);
        } else if (type == d) {
            return compute_global_average_double(graph, feature);
        } else if (type == i) {
            return compute_global_average_int(graph, feature);
        }
    }

    return sum / count;
}

uint64_t encode_comm_feature_pair(int community, int feature) {
    return ((uint64_t)community) << 32 | (uint64_t)feature;
}