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

    void read_edges(std::string edges_path);
    void read_features(std::string features_path);

    std::vector<double> get_features(int node);

    std::vector<int> get_neighbours(int node);
    std::vector<int> get_neighbours(int node, int depth);

    void print_edges();
    void print_features();
};

int get_num_nodes(std::string features_path);
int get_num_features(std::string features_path);