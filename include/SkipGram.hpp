#pragma once

#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test

#include <vector>

#include "DeepWalkImputer.hpp"
#include "Matrix.hpp"
#include "NegativeSampler.hpp"
#include "Vector.hpp"

class SkipGram {
    FRIEND_TEST(SkipGramTest, GenerateValidPairs);

   public:
    struct TrainingPair {
        int center;
        int context;
    };

    SkipGram(int num_nodes, DeepWalkImputer::DeepWalkConfig& config, int seed = -1);

    void train(const std::vector<std::vector<int>>& walks);

    Matrix get_embeddings();

   private:
    int num_nodes;
    DeepWalkImputer::DeepWalkConfig config;

    // storing W1 transposed because matrix data structure uses row-major storage, thus
    // get_row is more efficient then get_column, which we would have to use if we
    // didn't store W1 transposed
    // TODO: idea: handle this in the matrix data structure? (via a "transposed" flag e.g.)
    Matrix W1_T;
    Matrix W2;

    void process_pair(TrainingPair pair, double learning_rate, NegativeSampler& sampler);

    /**
     * @brief Generates training pairs from a random walk for the SkipGram model.
     *
     * Given a random walk (a sequence of node IDs) and a specified context window size,
     * this function creates training pairs for use in a SkipGram model. For each node in the
     * random walk, it pairs the node (as the center) with every node within its window,
     * excluding the center node itself. The resulting pairs can then be used to train
     * embeddings.
     *
     * @param random_walk A vector of integers representing the sequence of nodes from a random
     * walk.
     * @param window_size The number of nodes on each side of the center node to include in the
     * context.
     * @return std::vector<TrainingPair> A vector of training pairs, where each pair contains:
     *         - `center`: the center node from the random walk.
     *         - `context`: a context node within the specified window (excluding the center
     * node).
     */
    static std::vector<TrainingPair> generate_pairs(const std::vector<int>& random_walk,
                                                    int window_size);

    static double sigmoid(double val);

    // calculates linear decrease for each training pair (see word2vec paper)
    static double calculate_learning_rate_decrease(double learning_rate, int context_window,
                                                   int walk_length, int total_num_walks);
};