#include "Graph.hpp"

#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_set>

Graph::Graph(std::string edges_path) {
    num_nodes = parse_node_count(edges_path);  // TODO: make this work with the edge_file
    read_edges(edges_path);
}

Graph::Graph(std::vector<int> offsets, std::vector<int> edges) {
    this->offsets = offsets;
    this->edges = edges;
    num_nodes = offsets.size() - 1;
}

int Graph::get_num_nodes() const { return num_nodes; }

int Graph::get_num_edges() const { return edges.size() / 2; }

std::vector<int> Graph::get_neighbours(int node) const {
    if (!is_valid_node(node)) throw GraphException("Node does not exist");

    std::vector<int> neighbours(offsets[node + 1] - offsets[node]);
    std::copy(edges.begin() + offsets[node], edges.begin() + offsets[node + 1], neighbours.begin());

    return neighbours;
}

std::vector<int> Graph::get_neighbours(int node, int depth) const {
    if (!(is_valid_node(node))) throw GraphException("Node does not exist");

    std::unordered_set<int> visited;
    std::queue<int> frontier;
    frontier.push(node);
    visited.insert(node);

    for (int d = 0; d < depth; ++d) {
        int frontierSize = frontier.size();
        for (int i = 0; i < frontierSize; ++i) {
            int curr = frontier.front();
            frontier.pop();
            for (auto&& nbr : get_neighbours(curr)) {
                if (!visited.count(nbr)) {
                    visited.insert(nbr);
                    frontier.push(nbr);
                }
            }
        }
    }
    std::vector<int> neighbours(visited.begin(), visited.end());

    return neighbours;
}

int Graph::get_degree(int node) const {
    if (!(is_valid_node(node))) throw GraphException("Node does not exist");
    return offsets[node + 1] - offsets[node];
}

bool Graph::has_edge(int source, int target) const {
    if (!is_valid_node(source) || !is_valid_node(target))
        throw GraphException("Invalid node index.");
    // might be optimized by checking whether source's or target's adjacency list is smaller
    for (int i = offsets[source]; i < offsets[source + 1]; i++) {
        if (edges[i] == target) return true;
    }
    return false;
}

bool Graph::is_valid_node(int node) const { return node >= 0 && node < num_nodes; }

void Graph::read_edges(std::string edges_path) {
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