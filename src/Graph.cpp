#include "Graph.hpp"



//Default constructer sets number of nodes to 0
Graph::Graph() {
    n = 0;
}

Graph::Graph(int nodes) {
    this->n = nodes;
}
//Copies both number of nodes and the edges between them From the input Graph
Graph::Graph(const Graph &toCopy) {
    this->n = toCopy.n;
    this->edges = toCopy.edges;
}

//Deconstructer
Graph::~Graph() {
    //TODO
    //Might be empty
}

int Graph::numberOfNodes() const {return n;}


bool Graph::addEdge (int from, int to) {

    if (from >= 0 && to >= 0) {

        if (from < n && to < n) {
        edges.push_back(std::make_pair(from, to));
        return true;
        }       
    }
    return false;
}