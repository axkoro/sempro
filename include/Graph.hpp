#include <string>
#include <vector>

// TODO: Generic Class
class Graph {
   private:
    int num_nodes;

    std::vector<int> adj_array;
    std::vector<int> adj_array_indexes;

    std::vector<std::vector<double>> feature_list;

   public:
    Graph() {}
    Graph(std::string edges_path, std::string features_path);

    void readEdgesFromFile(std::string edges_path);
    void readFeaturesFromFile(std::string features_path);

    std::vector<double> getFeatures(int node);

    std::vector<int> getNeighbours(int node);
    std::vector<int> getNeighbours(int node, int depth);

    void printEdges();
    void printFeatures();
};