#pragma once

#include <random>
#include <vector>

/**
 * @brief Implements efficient negative sampling via the alias method.
 *
 * The NegativeSampler builds a probability distribution over nodes based on their
 * occurrences in a collection of random walks. Node frequencies are smoothed using a
 * given exponent (default 0.75) before the alias table is constructed for O(1) sampling.
 * This class is primarily intended for drawing negative samples (excluding a center node)
 * in the SkipGram model.
 */
class NegativeSampler {
   public:
    /**
     * @brief Constructs the NegativeSampler from a set of random walks.
     *
     * @param random_walks A vector of random walks, where each walk is represented as a vector of
     * node indices.
     * @param num_nodes Total number of nodes in the dataset.
     * @param smoothing_exponent Exponent to smooth the raw node counts (commonly 0.75).
     * @param seed Random seed for reproducibility; if -1, a non-deterministic seed is used.
     */
    NegativeSampler(const std::vector<std::vector<int>>& random_walks, int num_nodes,
                    double smoothing_exponent, int seed = -1);

    /**
     * @brief Draws a single sample from the negative sampling distribution.
     *
     * @return A node index sampled according to the precomputed alias table.
     */
    int draw_sample();

    /**
     * @brief Draws multiple negative samples while excluding a specified center node.
     *
     * @param center_node The node to be excluded from the negative samples.
     * @param num_samples Number of negative samples to draw.
     * @return A vector of sampled negative node indices.
     */
    std::vector<int> sample_negative_nodes(int center_node, int num_samples);

   private:
    /**
     * @brief Alias table for efficient sampling.
     *
     * 'aliases' holds the alternative indices for events with probability less than 1,
     * while 'probabilities' stores the normalized probability for choosing the index directly.
     */
    struct AliasTable {
        std::vector<int> aliases;
        std::vector<double> probabilities;
    } alias_table;

    std::mt19937 rng;
    std::uniform_int_distribution<>
        index_distribution;  ///< Distribution for selecting an alias table index.
    std::uniform_real_distribution<>
        probability_distribution;  ///< Distribution for selecting a probability value in [0, 1).

    /**
     * @brief Constructs an alias table from a given discrete probability distribution.
     *
     * The function scales probabilities by the number of events and partitions them into
     * "overfull" and "underfull" buckets, then constructs the alias table used for O(1) sampling.
     *
     * @param discrete_distribution A vector of probabilities that sum to 1.
     * @return An AliasTable with populated alias and probability arrays.
     */
    static AliasTable build_alias_table(std::vector<double> discrete_distribution);
};
