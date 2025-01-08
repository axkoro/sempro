#include "GraphDouble.hpp"

#include <fstream>
#include <iostream>

GraphDouble::GraphDouble(std::string edges_path, std::string features_path) {
    num_nodes = parse_node_count(features_path);
    num_features = parse_feature_count(features_path);
    read_edges(edges_path);
    read_features(features_path);
}

double GraphDouble::get_double_feature(int node, int feature) const {
    if (node < 0 || node >= num_nodes) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");

    return features[node][feature];
}

void GraphDouble::set_double_feature(int node, int feature, double value) {
    if (node < 0 || node >= num_nodes) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");

    features[node][feature] = value;
}

void GraphDouble::read_features(std::string features_path) {
    if (num_nodes == -1) {
        num_nodes = parse_node_count(features_path);
        if (num_nodes < 0)
            throw std::runtime_error("Couldn't parse node count from file: " + features_path);
    }
    if (num_features == -1) {
        num_features = parse_feature_count(features_path);
        if (num_nodes < 0)
            throw std::runtime_error("Couldn't parse feature count from file: " + features_path);
    }

    labels = std::vector<int>(num_nodes);
    features = std::vector<std::vector<double>>(num_nodes, std::vector<double>(num_features));
    missing = std::vector<std::vector<bool>>(num_nodes, std::vector<bool>(num_features));

    std::ifstream file(features_path);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + features_path);

    std::string line;
    while (std::getline(file, line)) {
        const char* ptr = line.c_str();
        char* endptr;

        // Parse node index
        int node = std::strtol(ptr, &endptr, 10);
        ptr = endptr;

        // Parse features
        for (int i = 0; i < num_features; i++) {
            double value = std::strtod(ptr, &endptr);
            if (ptr == endptr) {  // found '#'
                missing[node][i] = true;
                endptr += 3;  // advance three characters
            } else {
                features[node][i] = value;
            }
            ptr = endptr + 2;  // skip comma and space
        }

        // Parse label
        --ptr;  // because last feature and label are only separated by '\t'
        int label = std::strtol(ptr, &endptr, 10);
        labels[node] = label;
    }
    file.close();
}

void GraphDouble::print_features() const {
    for (int node = 0; node < num_nodes; node++) {
        std::cout << node << '\t';
        for (int i = 0; i < num_features - 1; i++) {
            if (missing[node][i]) {
                std::cout << "\'#\'" << ", ";
            } else {
                std::cout << features[node][i] << ", ";
            }
        }

        if (missing[node][num_features - 1]) {  // last feature
            std::cout << "\'#\'" << '\t';
        } else {
            std::cout << features[node][num_features - 1] << '\t';
        }

        std::cout << labels[node] << '\n';  // label
    }
}