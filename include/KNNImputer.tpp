#pragma once

#include <omp.h>

#include <cmath>
#include <mutex>
#include <type_traits>
#include <unordered_map>

#include "KNNImputer.hpp"  // because IntelliSense is stupid

template <typename T>
void KNNImputer<T>::run() {
    int num_nodes = this->graph.get_num_nodes();

    std::unordered_map<int, T> global_averages;
    std::mutex global_averages_mutex;

#pragma omp parallel for schedule(dynamic)
    for (int node = 0; node < num_nodes; ++node) {
        std::vector<int> neighbours;

        if (use_k_nearest) {
            neighbours = this->graph.get_k_nearest_neighbors(node, depth);  // Implement this
        } else {
            neighbours = this->graph.get_neighbours(node, depth);
        }
        auto missing_features = this->graph.get_missing_features(node);

        for (int feature : missing_features) {
            double sum = 0.0;
            int count = 0;

            for (int neighbour : neighbours) {
                if (!this->graph.is_missing(neighbour, feature)) {
                    T feature_val = this->graph.get_feature(neighbour, feature);
                    sum += static_cast<double>(feature_val);
                    ++count;
                }
            }

            if (count > 0) {
                T rounded_average = round_value<T>(sum / count);
                this->graph.set_feature(node, feature, rounded_average);
            } else {  // If no neighbor has the feature, use a global average.
                std::lock_guard<std::mutex> lock(global_averages_mutex);
                auto it = global_averages.find(feature);
                if (it != global_averages.end()) {
                    this->graph.set_feature(node, feature, it->second);
                } else {
                    double global_avg = Imputer<T>::compute_global_average(feature);
                    T imputed = round_value<T>(global_avg);
                    this->graph.set_feature(node, feature, imputed);
                    global_averages[feature] = imputed;
                }
            }
            this->graph.set_missing(node, feature, false);
        }
    }
}
