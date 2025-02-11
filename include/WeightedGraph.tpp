#pragma once

#include <queue>
#include <unordered_set>

// --- WeightedGraph Implementation ---

template <typename T>
WeightedGraph<T>::WeightedGraph(Graph<T>& graph)
    : graph(graph), edge_weights(graph.edges.size(), 1.0) {}

template <typename T>
WeightedEdgeIterator<T> WeightedGraph<T>::begin() {
    return WeightedEdgeIterator<T>(this, 0);
}

template <typename T>
WeightedEdgeIterator<T> WeightedGraph<T>::end() {
    return WeightedEdgeIterator<T>(this, edge_weights.size());
}

template <typename T>
std::vector<typename WeightedGraph<T>::Edge> WeightedGraph<T>::get_edges(int node) const {
    if (!(is_valid_node(node))) throw std::logic_error("Node does not exist");

    std::vector<Edge> neighbours(graph.offsets[node + 1] - graph.offsets[node]);
    size_t num_neighbours = neighbours.size();
    size_t offset = graph.offsets[node];
    for (size_t i = 0; i < num_neighbours; i++) {
        neighbours[i].target = graph.edges[offset + i];
        neighbours[i].weight = edge_weights[offset + i];
    }

    return neighbours;
}

template <typename T>
std::vector<typename WeightedGraph<T>::Edge> WeightedGraph<T>::get_edges(int node,
                                                                         int depth) const {
    if (!is_valid_node(node)) throw std::logic_error("Node does not exist");

    std::unordered_set<int> visited;
    std::queue<int> frontier;
    frontier.push(node);
    visited.insert(node);

    std::vector<Edge> result;

    for (int d = 0; d < depth; ++d) {
        int frontierSize = frontier.size();
        for (int i = 0; i < frontierSize; ++i) {
            int curr = frontier.front();
            frontier.pop();

            std::vector<Edge> currEdges = get_edges(curr);
            for (const auto& edge : currEdges) {
                if (!visited.count(edge.target)) {
                    visited.insert(edge.target);
                    frontier.push(edge.target);
                }
                result.push_back(edge);
            }
        }
    }

    return result;
}

// --- Forwarded Graph Methods ---

template <typename T>
int WeightedGraph<T>::get_num_nodes() const {
    return graph.get_num_nodes();
}

template <typename T>
int WeightedGraph<T>::get_num_features() const {
    return graph.get_num_features();
}

template <typename T>
int WeightedGraph<T>::get_num_edges() const {
    return graph.get_num_edges();
}

template <typename T>
T WeightedGraph<T>::get_feature(int node, int feature) const {
    return graph.get_feature(node, feature);
}

template <typename T>
int WeightedGraph<T>::get_label(int node) const {
    return graph.get_label(node);
}

template <typename T>
std::vector<int> WeightedGraph<T>::get_missing_features(int node) const {
    return graph.get_missing_features(node);
}

template <typename T>
int WeightedGraph<T>::get_degree(int node) const {
    return graph.get_degree(node);
}

template <typename T>
void WeightedGraph<T>::set_feature(int node, int feature, T value) {
    graph.set_feature(node, feature, value);
}

template <typename T>
void WeightedGraph<T>::set_missing(int node, int feature, bool value) {
    graph.set_missing(node, feature, value);
}

template <typename T>
bool WeightedGraph<T>::has_edge(int source, int target) const {
    return graph.has_edge(source, target);
}

template <typename T>
bool WeightedGraph<T>::is_missing(int node, int feature) const {
    return graph.is_missing(node, feature);
}

template <typename T>
bool WeightedGraph<T>::is_valid_node(int node) const {
    return graph.is_valid_node(node);
}

// --- WeightedEdgeIterator Implementation ---

template <typename T>
WeightedEdgeIterator<T>::WeightedEdgeIterator(WeightedGraph<T>* wg, size_t idx)
    : weighted_graph(wg), index(idx), current_node(0) {
    // Advance current_node until we find the node corresponding to index.
    while (current_node + 1 < weighted_graph->graph.offsets.size() &&
           index >= weighted_graph->graph.offsets[current_node + 1]) {
        ++current_node;
    }
}

template <typename T>
WeightedEdgeIterator<T>& WeightedEdgeIterator<T>::operator++() {
    ++index;
    if (current_node + 1 < weighted_graph->graph.offsets.size() &&
        index >= weighted_graph->graph.offsets[current_node + 1]) {
        ++current_node;
    }
    return *this;
}

template <typename T>
bool WeightedEdgeIterator<T>::operator!=(const WeightedEdgeIterator<T>& other) const {
    return index != other.index;
}

template <typename T>
double& WeightedEdgeIterator<T>::operator*() {
    return weighted_graph->edge_weights[index];
}

template <typename T>
std::pair<int, int> WeightedEdgeIterator<T>::get_edge() const {
    return {static_cast<int>(current_node), weighted_graph->graph.edges[index]};
}