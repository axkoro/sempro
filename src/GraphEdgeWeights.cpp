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

    std::vector<Edge> neighbors(graph.offsets[node + 1] - graph.offsets[node]);
    size_t num_neighbors = neighbors.size();
    size_t offset = graph.offsets[node];
    for (size_t i = 0; i < num_neighbors; i++) {
        neighbors[i].target = graph.edges[offset + i];
        neighbors[i].weight = edge_weights[offset + i];
    }

    return neighbors;
}

std::vector<typename GraphEdgeWeights::Edge> GraphEdgeWeights::get_edges(int node,
                                                                         int depth) const {
    if (!graph.is_valid_node(node)) throw std::logic_error("Node does not exist");

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

// --- WeightedEdgeIterator Implementation ---

WeightedEdgeIterator::WeightedEdgeIterator(GraphEdgeWeights* wg, size_t idx)
    : weighted_graph(wg), index(idx), current_node(0) {
    // Advance current_node until we find the node corresponding to index.
    while (current_node + 1 < weighted_graph->graph.offsets.size() &&
           index >= weighted_graph->graph.offsets[current_node + 1]) {
        ++current_node;
    }
}

WeightedEdgeIterator& WeightedEdgeIterator::operator++() {
    ++index;
    if (current_node + 1 < weighted_graph->graph.offsets.size() &&
        index >= weighted_graph->graph.offsets[current_node + 1]) {
        ++current_node;
    }
    return *this;
}

bool WeightedEdgeIterator::operator!=(const WeightedEdgeIterator& other) const {
    return index != other.index;
}

double& WeightedEdgeIterator::operator*() { return weighted_graph->edge_weights[index]; }

std::pair<int, int> WeightedEdgeIterator::get_edge() const {
    return {static_cast<int>(current_node), weighted_graph->graph.edges[index]};
}