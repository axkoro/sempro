#include <vector>

class NodeDistribution {
   public:
    // builds the distribution of node occurrences within a list of random walks
    NodeDistribution(const std::vector<std::vector<int>>& random_walks, int num_nodes);

    int draw_sample();

   private:
};