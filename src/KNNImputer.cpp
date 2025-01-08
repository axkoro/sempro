#include "KNNImputer.hpp"

#include <omp.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <unordered_map>

#include "GraphBool.hpp"
#include "GraphDouble.hpp"
#include "GraphInt.hpp"

void KNNImputer::set_depth(int k) { this->k = k; }
int KNNImputer::get_depth() { return k; }

void KNNImputer::run() {
    int num_nodes = graph.get_num_nodes();
    int num_features = graph.get_num_features();

    if (type == b) {
        std::unordered_map<int, bool> global_averages;  // stores global averages for each feature

#pragma omp parallel for
        // for each node in graph if feature is missing take the average of this feature among the
        // neighbourhood with depth k
        for (int node = 0; node < num_nodes; node++) {
            std::vector<int> neighbours = graph.get_neighbours(node, k);

            std::vector<int> missing_features = graph.get_missing_features(node);
            for (auto&& feature : missing_features) {
                int sum = 0;
                int count = 0;

                for (int neighbour : neighbours) {
                    if (!graph.is_missing(neighbour, feature)) {
                        sum += graph.get_bool_feature(neighbour, feature);
                        count++;
                    }
                }

                if (count > 0) {
                    graph.set_bool_feature(node, feature, to_bool((double)sum / (double)count));
                } else {  // if no neighbours have the feature, use global average
                    auto it = global_averages.find(feature);
                    if (it != global_averages.end()) {
                        graph.set_bool_feature(node, feature, it->second);
                    } else {
                        double global_avg = compute_global_average_bool(graph, feature);
                        graph.set_bool_feature(node, feature, global_avg);

                        std::mutex avg_mutex;
                        {
                            std::lock_guard<std::mutex> lock(avg_mutex);
                            global_averages[feature] = global_avg;
                        }
                    }
                }

                // this leads to imputed features being used for further imputations
                graph.set_missing(node, feature, false);
            }
        }
    } else if (type == d) {
        std::unordered_map<int, double> global_averages;  // stores global averages for each feature

#pragma omp parallel for
        // for each node in graph if feature is missing take the average of this feature among the
        // neighbourhood with depth k
        for (int node = 0; node < num_nodes; node++) {
            std::vector<int> neighbours = graph.get_neighbours(node, k);

            std::vector<int> missing_features = graph.get_missing_features(node);
            for (auto&& feature : missing_features) {
                double sum = 0;
                int count = 0;

                for (int neighbour : neighbours) {
                    if (!graph.is_missing(neighbour, feature)) {
                        sum += graph.get_double_feature(neighbour, feature);
                        count++;
                    }
                }

                if (count > 0) {
                    graph.set_double_feature(node, feature, sum / count);
                } else {  // if no neighbours have the feature, use global average
                    auto it = global_averages.find(feature);
                    if (it != global_averages.end()) {
                        graph.set_double_feature(node, feature, it->second);
                    } else {
                        double global_avg = compute_global_average_double(graph, feature);
                        graph.set_double_feature(node, feature, global_avg);

                        std::mutex avg_mutex;
                        {
                            std::lock_guard<std::mutex> lock(avg_mutex);
                            global_averages[feature] = global_avg;
                        }
                    }
                }

                // this leads to imputed features being used for further imputations
                graph.set_missing(node, feature, false);
            }
        }
    } else if (type == i) {
        std::unordered_map<int, int> global_averages;  // stores global averages for each feature

#pragma omp parallel for
        // for each node in graph if feature is missing take the average of this feature among the
        // neighbourhood with depth k
        for (int node = 0; node < num_nodes; node++) {
            std::vector<int> neighbours = graph.get_neighbours(node, k);

            std::vector<int> missing_features = graph.get_missing_features(node);
            for (auto&& feature : missing_features) {
                int sum = 0;
                int count = 0;

                for (int neighbour : neighbours) {
                    if (!graph.is_missing(neighbour, feature)) {
                        sum += graph.get_int_feature(neighbour, feature);
                        count++;
                    }
                }

                if (count > 0) {
                    graph.set_int_feature(node, feature, to_int((double)sum / (double)count));
                } else {  // if no neighbours have the feature, use global average
                    auto it = global_averages.find(feature);
                    if (it != global_averages.end()) {
                        graph.set_int_feature(node, feature, it->second);
                    } else {
                        double global_avg = compute_global_average_int(graph, feature);
                        graph.set_int_feature(node, feature, global_avg);

                        std::mutex avg_mutex;
                        {
                            std::lock_guard<std::mutex> lock(avg_mutex);
                            global_averages[feature] = global_avg;
                        }
                    }
                }

                // this leads to imputed features being used for further imputations
                graph.set_missing(node, feature, false);
            }
        }
    }
}

// Compute the global average of a feature across all nodes in the graph.
double compute_global_average_double(Graph& graph, int feature) {
    double sum = 0;
    int count = 0;
    for (int node = 0; node < graph.get_num_nodes(); node++) {
        if (!graph.is_missing(node, feature)) {
            sum += graph.get_double_feature(node, feature);
            count++;
        }
    }

    return (count > 0) ? (sum / count) : 0;
}

double compute_global_average_bool(Graph& graph, int feature) {  // TODO: round
    double sum = 0;
    int count = 0;
    for (int node = 0; node < graph.get_num_nodes(); node++) {
        if (!graph.is_missing(node, feature)) {
            sum += graph.get_bool_feature(node, feature);
            count++;
        }
    }

    return (count > 0) ? (to_bool(sum / count)) : 0;
}

double compute_global_average_int(Graph& graph, int feature) {  // TODO: round
    double sum = 0;
    int count = 0;
    for (int node = 0; node < graph.get_num_nodes(); node++) {
        if (!graph.is_missing(node, feature)) {
            sum += graph.get_int_feature(node, feature);
            count++;
        }
    }

    return (count > 0) ? (to_int(sum / count)) : 0;
}

bool to_bool(double value) { return value >= 0.5 ? 1 : 0; }

int to_int(double value) { return std::lround(value); }