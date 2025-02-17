#pragma once

#include <stdexcept>
#include <string>
#include <vector>

class GraphException : public std::runtime_error {
   public:
    explicit GraphException(const std::string& message) : std::runtime_error(message) {};
};

class GraphEdgeWeights;
class WeightedEdgeIterator;

class Graph {
    friend class GraphEdgeWeights;
    friend class WeightedEdgeIterator;

   protected:
    int num_nodes;
    std::vector<int> offsets;
    std::vector<int> edges;

   public:
    /**
     * @brief Default constructor. Creates an empty graph.
     */
    Graph() = default;

    /**
     * @brief Constructs a graph by reading edge and feature files.
     *
     * @param edges_path Path to the file containing edges in an edge list format.
     */
    Graph(std::string edges_path);

    /**
     * @brief Debugging/testing constructor for creating a Graph without features.
     */
    Graph(std::vector<int> offsets, std::vector<int> edges);  // TODO: remove

    // Getters

    int get_num_nodes() const;
    int get_num_edges() const;

    /**
     * @brief Gets the immediate neighbours of a node.
     *
     * @param node Index of the node.
     * @return Vector of neighbour node indices.
     * @throws std::logic_error If the node does not exist.
     */
    std::vector<int> get_neighbours(int node) const;  // TODO: use iterator instead

    /**
     * @brief Gets k random neighbours of a node.
     *
     * @param node Index of the node.
     * @param k Number of neighbours to return.
     * @return Vector of neighbour node indices.
     * @throws std::logic_error If the node does not exist.
     */
    std::vector<int> get_k_nearest_neighbors(int node, int k);

    /**
     * @brief Gets the neighbours of a node up to a certain depth.
     *
     * @param node Index of the node.
     * @param depth Depth of neighbour search.
     * @return Vector of neighbour node indices.
     * @throws std::logic_error If the node does not exist.
     */
    std::vector<int> get_neighbours(int node, int depth) const;  // TODO: ? use iterator instead

    /**
     * @brief Gets the degree of a node.
     *
     * @param node Index of the node.
     * @return Degree of the node.
     * @throws std::logic_error If the node does not exist.
     */
    int get_degree(int node) const;

    // Queries

    /**
     * @brief Checks if an edge exists between two nodes.
     *
     * @param source Index of the source node.
     * @param target Index of the target node.
     * @return True if the edge exists, false otherwise.
     */
    bool has_edge(int source, int target) const;

    /**
     * @brief Validates if a node index is within the graph.
     *
     * @param node Index of the node.
     * @return True if the node is valid, false otherwise.
     */
    bool is_valid_node(int node) const;

    // Debug/Display

    /**
     * @brief Prints unique edges of the graph to the console.
     */
    void print_edges() const;

   protected:
    /**
     * @brief Reads edges from a file and builds the graph.
     *
     * @param edges_path Path to the file containing edges.
     * @throws std::runtime_error If the number of nodes is uninitialized or file errors occur.
     */
    void read_edges(std::string edges_path);

   private:
    /**
     * @brief Parses the number of nodes from the features file.
     *
     * @param edges_path Path to the edges file.
     * @return Total number of nodes.
     * @throws std::runtime_error If file errors occur or parsing fails.
     */
    static int parse_node_count_from_edge_file(std::string edges_path);
};