#include <string>
#include <vector>

// TODO: Generic Class
class Graph {
   private:
    int num_nodes;
    int num_features;

    std::vector<int> offsets;
    std::vector<int> edges;

    std::vector<std::vector<double>> features;  // includes label as last feature
    std::vector<std::vector<bool>> missing;

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
    
    std::vector<std::vector<double>> getFeatures(); //neu
    std::vector<std::vector<bool>> getMissing(); //neu

    int get_num_nodes(); //neu
    int get_num_features(); //neu 
};

int getNumNodes(std::string features_path);
int getNumFeatures(std::string features_path);