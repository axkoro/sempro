#ifndef GRAPH_UTILS_HPP
#define GRAPH_UTILS_HPP

#include <iostream>
#include <vector>
#include "Graph.hpp"



namespace GraphUtil {


    // Function to extract a subgraph given a set of nodes
    Graph extractSubgraph(const Graph& graph, const std::vector<int>& nodeSet);

    // Function to calculate the adjacency matrix
    std::vector<std::vector<int>> calculateAdjacencyMatrix(const Graph& graph);

    // Utility function to check graph connectivity
    bool isConnected(const Graph& graph);

    // Function to read graph data from an edge list file
    Graph readGraphFromEdgeList(const std::string& fileName);

    // Function to write graph data to an edge list file
    void writeGraphToEdgeList(const Graph& graph, const std::string& fileName);
}


#endif