#include <random>
#include <vector>

class NegativeSampler {
   public:
    // builds the distribution of node occurrences within a list of random walks
    // default value for smoothing exponent: 0.75
    NegativeSampler(const std::vector<std::vector<int>>& random_walks, int num_nodes,
                    double smoothing_exponent, int seed = -1);

    int draw_sample();
    std::vector<int> sample_negative_nodes(int center_node, int num_samples);

   private:
    struct AliasTable {
        std::vector<int> aliases;
        std::vector<double> probabilities;
    } alias_table;

    std::mt19937 rng;
    std::uniform_int_distribution<> int_dist;
    std::uniform_real_distribution<> real_dist;

    // requirement: discrete_distribution holds probabilities that add up to 1.0
    static AliasTable build_alias_table(std::vector<double> discrete_distribution);
};