#pragma once

#include <algorithm>
#include <charconv>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <type_traits>
#include <unordered_set>

#include "AttributedGraph.hpp"  // because IntelliSense is stupid

template <typename T>
AttributedGraph<T>::AttributedGraph(std::string edges_path, std::string features_path) {
    if constexpr (!std::is_arithmetic_v<T>) {
        throw std::domain_error(
            "AttributedGraph class does not support non-arithmetic node attribute types");
    }

    num_nodes = parse_node_count_from_feature_file(features_path);
    num_features = parse_feature_count(features_path);
    read_edges(edges_path);
    read_features(features_path);
}

template <typename T>
int AttributedGraph<T>::get_num_features() const {
    return num_features;
}

template <typename T>
T AttributedGraph<T>::get_feature(int node, int feature) const {
    if (!(is_valid_node(node))) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");
    return features[node][feature];
}

template <typename T>
int AttributedGraph<T>::get_label(int node) const {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    return labels[node];
}

template <typename T>
void AttributedGraph<T>::set_feature(int node, int feature, T value) {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");
    features[node][feature] = value;
}

template <typename T>
void AttributedGraph<T>::set_missing(int node, int feature, bool value) {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");
    missing[node][feature] = value;
}

template <typename T>
std::vector<int> AttributedGraph<T>::get_missing_features(int node) const {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    std::vector<int> missing_features_list;
    for (int feature = 0; feature < num_features; feature++) {
        if (is_missing(node, feature)) missing_features_list.push_back(feature);
    }

    return missing_features_list;
}

template <typename T>
bool AttributedGraph<T>::is_missing(int node, int feature) const {
    if (!is_valid_node(node)) throw GraphException("Invalid node index.");
    if (feature < 0 || feature >= num_features) throw GraphException("Invalid feature index.");
    return missing[node][feature];
}

template <typename T>
void AttributedGraph<T>::read_features(std::string features_path) {
    labels = std::vector<int>(num_nodes);
    features = std::vector<std::vector<T>>(num_nodes, std::vector<T>(num_features));
    missing = std::vector<std::vector<bool>>(num_nodes, std::vector<bool>(num_features));

    std::ifstream file(features_path);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + features_path);

    std::string line;
    while (std::getline(file, line)) {
        parse_features_line(line);
    }
    file.close();
}

template <typename T>
void AttributedGraph<T>::print_features() const {
    std::cout << std::noboolalpha;  // print 1/0 instead of true/false

    for (int node = 0; node < num_nodes; node++) {
        std::cout << node << '\t';
        for (int i = 0; i < num_features - 1; i++) {
            if (missing[node][i]) {
                std::cout << "#" << ", ";
            } else {
                std::cout << features[node][i] << ", ";
            }
        }

        if (missing[node][num_features - 1]) {  // last feature
            std::cout << "#" << '\t';
        } else {
            std::cout << features[node][num_features - 1] << '\t';
        }

        std::cout << labels[node] << '\n';  // label
    }
}

template <typename T>
void AttributedGraph<T>::print_features_to_file(std::string output_path) const {
    std::ofstream file(output_path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + output_path);

    std::streambuf* coutbuf = std::cout.rdbuf();  // save old buf
    std::cout.rdbuf(file.rdbuf());                // redirect std::cout to file

    print_features();

    std::cout.rdbuf(coutbuf);  // reset to standard output again
    file.close();
}

template <typename T>
void AttributedGraph<T>::parse_features_line(std::string& line) {
    const char* ptr = line.c_str();
    char* endptr;

    // Parse node index
    int node = std::strtol(ptr, &endptr, 10);
    ptr = endptr;

    // Parse features
    if constexpr (std::is_integral_v<T>) {
        for (int i = 0; i < num_features; i++) {
            long value = std::strtol(ptr, &endptr, 10);
            if (ptr == endptr) {  // found #
                missing[node][i] = true;
                endptr += 1;              // skip the #
            } else if (*endptr == '.') {  // found value with comma
                features[node][i] = value;

                // skip decimal places
                if (i != num_features - 1) {
                    endptr += strcspn(endptr, ",");
                } else {
                    endptr += strcspn(endptr, "\t");
                }
            } else {  // found value without comma
                features[node][i] = value;
            }
            ptr = endptr + 2;  // skip comma and space
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        for (int i = 0; i < num_features; i++) {
            double value = std::strtod(ptr, &endptr);
            if (ptr == endptr) {  // found #
                missing[node][i] = true;
                endptr += 1;  // skip the #
            } else {
                features[node][i] = value;
            }
            ptr = endptr + 2;  // skip comma and space
        }
    } else {
        throw std::domain_error("Can't parse non-numeric types");
    }

    // Parse label
    --ptr;  // because last feature and label are only separated by '\t'
    labels[node] = std::strtol(ptr, NULL, 10);
}

template <typename T>
int AttributedGraph<T>::parse_node_count_from_feature_file(std::string features_path) {
    std::ifstream file(features_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + features_path);
    }

    // iterate backwards to find beginning of last line
    file.seekg(-2, file.end);  // -2 because -1 would be '\n' (see assumptions)
    size_t pos = file.tellg();
    while (file.get() != '\n') {
        file.seekg(--pos);
    }

    // get node from last line
    std::string last_line;
    if (!std::getline(file, last_line)) {
        throw std::runtime_error("Failed to read the last line");
    }

    std::istringstream line_stream(last_line);
    int node;
    if (!(line_stream >> node)) {
        throw std::runtime_error("Failed to parse integer");
    }

    file.close();

    if (node < 0) {
        throw std::runtime_error("Invalid node value");
    }

    return node + 1;  // assuming naming starts at 0
}

template <typename T>
int AttributedGraph<T>::parse_feature_count(std::string features_path) {
    std::ifstream file(features_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + features_path);
    }

    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Failed to read the first line");
    }

    int comma_count = std::count(line.begin(), line.end(), ',');
    if (comma_count == 0)
        throw std::runtime_error("Invalid format: no commas found in the line of file '" +
                                 features_path + "'");
    return comma_count + 1;  // assuming one comma after every feature except the last
}