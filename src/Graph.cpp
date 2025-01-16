#include "Graph.hpp"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

GraphException::GraphException(const std::string& message) : std::runtime_error(message) {}

int Graph::get_num_nodes() const { return num_nodes; }

int Graph::get_num_features() const { return num_features; }

int Graph::get_num_edges() const { return edges.size(); }

int Graph::get_label(int node) const { return labels[node]; }

void Graph::set_missing(int node, int feature, bool value) { missing[node][feature] = value; }

std::vector<int> Graph::get_missing_features(int node) const {
    std::vector<int> missing_features_list;
    for (int feature = 0; feature < num_features; feature++) {
        if (is_missing(node, feature)) missing_features_list.push_back(feature);
    }

    return missing_features_list;
}

std::vector<int> Graph::get_neighbours(int node) const {
    if (!(is_valid_node(node))) throw std::logic_error("Node does not exist");

    std::vector<int> neighbours(offsets[node + 1] - offsets[node]);
    std::copy(edges.begin() + offsets[node], edges.begin() + offsets[node + 1], neighbours.begin());

    return neighbours;
}

std::vector<int> Graph::get_neighbours(int node, int depth) const {
    if (!(is_valid_node(node))) throw std::logic_error("Node does not exist");

    std::vector<int> neighbours = get_neighbours(node);  // k=1

    int last_depth_start = 0;
    int this_depth_start = neighbours.size();
    for (int k = 2; k <= depth; k++) {
        for (int i = last_depth_start; i < this_depth_start; i++) {
            std::vector<int> new_neighbours = get_neighbours(neighbours[i]);
            neighbours.insert(neighbours.end(), new_neighbours.begin(), new_neighbours.end());
        }
        neighbours = remove_duplicates(neighbours);
        last_depth_start = this_depth_start;
        this_depth_start = neighbours.size();
    }

    return neighbours;
}

int Graph::get_degree(int node) const {
    if (!(is_valid_node(node))) throw std::logic_error("Node does not exist");
    return offsets[node + 1] - offsets[node];
}

bool Graph::has_edge(int source, int target) const {
    // might be optimized by checking whether source's or target's adjacency list is smaller
    for (int i = offsets[source]; i < offsets[source + 1]; i++) {
        if (edges[i] == target) return true;
    }
    return false;
}

bool Graph::is_valid_node(int node) const { return node >= 0 && node < num_nodes; }

bool Graph::is_missing(int node, int feature) const { return missing[node][feature]; }

void Graph::read_edges(std::string edges_path) {
    if (num_nodes == -1) {
        throw std::runtime_error("num_nodes needs to be initialised before calling read_edges");
    }

    std::ifstream file(edges_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + edges_path);
    }

    std::vector<std::vector<int>> temp_edges(num_nodes);  // temporary adjecency list

    int a, b;
    while (file >> a >> b) {
        if (!is_valid_node(a) || !is_valid_node(b)) {
            throw std::out_of_range("Node index out of range");
        }
        temp_edges[a].push_back(b);
        temp_edges[b].push_back(a);
    }

    if (file.bad()) {
        throw std::runtime_error("Error reading file: " + edges_path);
    }

    // Flatten to CSR
    offsets = std::vector<int>(num_nodes + 1);
    for (int i = 1; i < offsets.size(); i++) {
        offsets[i] = offsets[i - 1] + temp_edges[i - 1].size();
    }

    int num_edges = offsets[offsets.size() - 1];
    edges = std::vector<int>(num_edges);
    for (int i = 0; i < temp_edges.size(); i++) {
        std::copy(temp_edges[i].begin(), temp_edges[i].end(), edges.begin() + offsets[i]);
    }

    file.close();
}

void Graph::print_edges() const {
    for (int i = 0; i < num_nodes; i++) {
        for (int j = offsets[i]; j < offsets[i + 1]; j++) {
            int neighbour = edges[j];
            if (neighbour <= i) {  // to only print unique edges (in descending order)
                std::cout << i << "\t" << neighbour << "\n";
            }
        }
    }
}

void Graph::print_features_to_file(std::string output_path) const {
    std::ofstream file(output_path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + output_path);

    std::streambuf* coutbuf = std::cout.rdbuf();  // save old buf
    std::cout.rdbuf(file.rdbuf());                // redirect std::cout to file

    print_features();

    std::cout.rdbuf(coutbuf);  // reset to standard output again
    file.close();
}

int parse_node_count(std::string features_path) {
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

int parse_feature_count(std::string features_path) {
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

std::vector<int> remove_duplicates(const std::vector<int>& arr) {
    std::unordered_set<int> seen;
    std::vector<int> result;

    for (const int& num : arr) {
        if (seen.insert(num).second) {  // Only insert if not already in set
            result.push_back(num);
        }
    }

    return result;
}