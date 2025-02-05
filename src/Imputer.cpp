#include "Imputer.hpp"

#include <cmath>

#include "Graph.hpp"

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

double compute_global_average_bool(Graph& graph, int feature) {
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

double compute_global_average_int(Graph& graph, int feature) {
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