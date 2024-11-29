#include "Graph.hpp"

#include <fstream>
#include <sstream>

void addEdge(int a, int b) {}

void Graph::readEdgesFromFile(std::string edges_path) {
    std::ifstream file(edges_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int a, b;
        if (!(iss >> a >> b)) {
            break;
        }
        addEdge(a, b);
    }

    file.close();
}
