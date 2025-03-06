#include "GraphEdgeWeights.hpp"

#include <queue>
#include <unordered_set>

// --- GraphEdgeWeights Implementation ---

GraphEdgeWeights::GraphEdgeWeights(Graph& graph)
    : graph(graph), edge_weights(graph.edges.size(), 1.0) {}

WeightedEdgeIterator GraphEdgeWeights::begin() { return WeightedEdgeIterator(this, 0); }

WeightedEdgeIterator GraphEdgeWeights::end() {
    return WeightedEdgeIterator(this, edge_weights.size());
}

std::vector<GraphEdgeWeights::Edge> GraphEdgeWeights::get_edges(int node) const {
    if (!(graph.is_valid_node(node))) throw std::logic_error("Node does not exist");

    size_t num_neighbors = graph.offsets[node + 1] - graph.offsets[node];
    std::vector<Edge> neighbors;
    neighbors.reserve(num_neighbors);

    size_t offset = graph.offsets[node];
    for (size_t i = 0; i < num_neighbors; i++) {
        int target = graph.edges[offset + i];
        double weight = edge_weights[offset + i];
        neighbors.emplace_back(target, weight);
    }

    return neighbors;
}

// --- WeightedEdgeIterator Implementation ---

WeightedEdgeIterator::WeightedEdgeIterator(GraphEdgeWeights* wg, size_t idx)
    : weighted_graph(wg), index(idx), current_node(0) {
    while (current_node + 1 < weighted_graph->graph.offsets.size() &&
           index >= weighted_graph->graph.offsets[current_node + 1]) {
        ++current_node;
    }
}

void WeightedEdgeIterator::update_current_node() {
    while (current_node + 1 < weighted_graph->graph.offsets.size() &&
           index >= weighted_graph->graph.offsets[current_node + 1]) {
        ++current_node;
    }
    while (current_node > 0 && index < weighted_graph->graph.offsets[current_node]) {
        --current_node;
    }
}

WeightedEdgeIterator& WeightedEdgeIterator::operator++() {
    ++index;
    update_current_node();
    return *this;
}

WeightedEdgeIterator WeightedEdgeIterator::operator++(int) {
    WeightedEdgeIterator tmp = *this;
    ++(*this);
    return tmp;
}

WeightedEdgeIterator& WeightedEdgeIterator::operator--() {
    --index;
    update_current_node();
    return *this;
}

WeightedEdgeIterator WeightedEdgeIterator::operator--(int) {
    WeightedEdgeIterator tmp = *this;
    --(*this);
    return tmp;
}

WeightedEdgeIterator& WeightedEdgeIterator::operator+=(difference_type n) {
    index += n;
    update_current_node();
    return *this;
}

WeightedEdgeIterator& WeightedEdgeIterator::operator-=(difference_type n) {
    index -= n;
    update_current_node();
    return *this;
}

WeightedEdgeIterator WeightedEdgeIterator::operator+(difference_type n) const {
    WeightedEdgeIterator tmp = *this;
    tmp += n;
    return tmp;
}

WeightedEdgeIterator WeightedEdgeIterator::operator-(difference_type n) const {
    WeightedEdgeIterator tmp = *this;
    tmp -= n;
    return tmp;
}

WeightedEdgeIterator::difference_type WeightedEdgeIterator::operator-(
    const WeightedEdgeIterator& other) const {
    return static_cast<difference_type>(index) - static_cast<difference_type>(other.index);
}

WeightedEdgeIterator::reference WeightedEdgeIterator::operator*() {
    return weighted_graph->edge_weights[index];
}

WeightedEdgeIterator::pointer WeightedEdgeIterator::operator->() { return &(**this); }

WeightedEdgeIterator::reference WeightedEdgeIterator::operator[](difference_type n) {
    return *(*this + n);
}

bool WeightedEdgeIterator::operator==(const WeightedEdgeIterator& other) const {
    return index == other.index;
}

bool WeightedEdgeIterator::operator!=(const WeightedEdgeIterator& other) const {
    return index != other.index;
}

bool WeightedEdgeIterator::operator<(const WeightedEdgeIterator& other) const {
    return index < other.index;
}

bool WeightedEdgeIterator::operator<=(const WeightedEdgeIterator& other) const {
    return index <= other.index;
}

bool WeightedEdgeIterator::operator>(const WeightedEdgeIterator& other) const {
    return index > other.index;
}

bool WeightedEdgeIterator::operator>=(const WeightedEdgeIterator& other) const {
    return index >= other.index;
}

std::pair<int, int> WeightedEdgeIterator::get_edge() const {
    int source = static_cast<int>(current_node);
    size_t offset = weighted_graph->graph.offsets[current_node];
    int target = weighted_graph->graph.edges[index];
    return {source, target};
}