#pragma once

/**
 * @brief Macro to allow friend tests.
 *
 * This macro declares a friend test class for a given test case and test name.
 */
#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test

#include <vector>

#include "Configs.hpp"
#include "Matrix.hpp"
#include "NegativeSampler.hpp"

/**
 * @brief Implements the SkipGram model for learning node embeddings.
 *
 * The SkipGram model generates training pairs from random walks and updates embedding matrices
 * using negative sampling.
 */
class SkipGram {
    FRIEND_TEST(SkipGramTest, GenerateValidPairs);

   public:
    /**
     * @brief Represents a pair of training nodes.
     *
     * A training pair consists of a center node and a context node.
     */
    struct TrainingPair {
        int center;   ///< Index of the center node.
        int context;  ///< Index of the context node.
        TrainingPair(int center, int context) : center(center), context(context) {};
    };

    /**
     * @brief Constructs a SkipGram model.
     *
     * @param num_nodes Total number of nodes.
     * @param config Configuration parameters for training.
     * @param seed Random seed for reproducibility; if -1, a non-deterministic seed is used.
     */
    SkipGram(int num_nodes, SkipGramConfig& config, int seed = -1);

    /**
     * @brief Trains the SkipGram model using provided random walks.
     *
     * @note All random walks must be of the same length; otherwise, the behavior is undefined.
     *
     * @param walks A vector of random walks, each represented as a vector of node IDs.
     *
     * @throws std::logic_error if walks is empty.
     */
    void train(const std::vector<std::vector<int>>& walks);

    /**
     * @brief Retrieves the learned node embeddings.
     *
     * @note Each row (!) of the returned Matrix corresponds to an embedding vector
     *       (which is atypical compared to the conventional column-based representation).
     *
     * @return Matrix containing the node embeddings.
     */
    Matrix get_embeddings() const;

   private:
    int num_nodes;          ///< Total number of nodes.
    int seed;               ///< Seed for random number generation.
    SkipGramConfig config;  ///< Training configuration parameters.

    /**
     * @brief Embedding matrix for input nodes (stored transposed).
     *
     * Storing W1 transposed is more efficient for row-based access.
     * TODO: idea: handle this in the matrix data structure? (using a "transposed" flag e.g.)
     */
    Matrix W1_T;

    Matrix W2;  ///< Embedding matrix for output nodes.

    /**
     * @brief Processes a single training pair and updates embeddings.
     *
     * Performs forward and backward passes using negative sampling.
     *
     * @param pair The training pair to process.
     * @param learning_rate The current learning rate.
     * @param sampler Reference to a NegativeSampler for sampling negative nodes.
     */
    void process_pair(TrainingPair pair, double learning_rate, NegativeSampler& sampler);

    /**
     * @brief Generates training pairs from a random walk.
     *
     * For each node in the random walk, the function pairs it with every node within the
     * specified context window, excluding the pair where the center node is paired with itself
     * at the center position.
     *
     * @note Only the immediate self-pair is excluded. If the same node appears at a different
     *       position within the context window, the pair (center, context) with center == context
     *       is still generated.
     *
     * @param random_walk A vector of node IDs representing a random walk.
     * @param window_size The size of the context window on each side of the center node.
     * @return std::vector<TrainingPair> A vector of generated training pairs.
     */
    static std::vector<TrainingPair> generate_pairs(const std::vector<int>& random_walk,
                                                    int window_size);

    /**
     * @brief Computes the sigmoid function.
     *
     * @param val Input value.
     * @return double Result of sigmoid(val).
     */
    static double sigmoid(double val);

    /**
     * @brief Calculates the linear decrease in learning rate per training pair.
     *
     * Based on the word2vec paper, the learning rate decreases linearly over training pairs.
     *
     * @param learning_rate The initial learning rate.
     * @param context_window The context window size.
     * @param walk_length The length of each random walk.
     * @param total_num_walks Total number of random walks.
     * @return double The decrease in learning rate per training pair.
     */
    static double calculate_learning_rate_decrease(double learning_rate, int context_window,
                                                   int walk_length, int total_num_walks);

   public:
    static double dot_product(std::span<const double> a, std::span<const double> b);
};
