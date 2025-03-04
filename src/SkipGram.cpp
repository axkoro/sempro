#include "SkipGram.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

SkipGram::SkipGram(int num_nodes, SkipGramConfig& config, int seed)
    : num_nodes(num_nodes), config(config), seed(seed) {
    std::mt19937 rng(seed == -1 ? std::random_device{}() : seed);
    double limit = 0.5 / config.embedding_size;
    std::uniform_real_distribution<double> noise_dist(-limit, limit);
    auto distribution = [&rng, &noise_dist]() { return noise_dist(rng); };

    W1_T = Matrix(num_nodes, config.embedding_size, distribution);
    W2 = Matrix(num_nodes, config.embedding_size, 0.0);
}

void SkipGram::train(const std::vector<std::vector<int>>& walks) {
    if (walks.empty()) throw std::logic_error("Cannot train using empty walks");

    double learning_rate = config.learning_rate;
    double learning_rate_decrease = calculate_learning_rate_decrease(
        learning_rate, config.context_window, walks[0].size(),
        walks.size());  // for linear decrease per training pair (see word2vec paper)

    NegativeSampler sampler(walks, num_nodes, config.smoothing_exponent, seed);
    std::mt19937 rng(seed == -1 ? std::random_device{}() : seed);

    for (int epoch = 0; epoch < config.num_epochs; epoch++) {
        std::vector<size_t> walk_indices(walks.size());
        std::iota(walk_indices.begin(), walk_indices.end(), 0);
        std::shuffle(walk_indices.begin(), walk_indices.end(), rng);

        for (size_t idx : walk_indices) {
            std::vector<TrainingPair> pairs = generate_pairs(walks[idx], config.context_window);
            std::shuffle(pairs.begin(), pairs.end(), rng);

            for (const auto& pair : pairs) {
                process_pair(pair, learning_rate, sampler);
                learning_rate -= learning_rate_decrease;
            }
        }
    }
}

void SkipGram::process_pair(TrainingPair pair, double learning_rate, NegativeSampler& sampler) {
    auto negatives = sampler.sample_negative_nodes(pair.center, config.num_negative_samples);

    auto v_c = W1_T.get_row(pair.center);  // center embedding
    auto v_o = W2.get_row(pair.context);   // context embedding

    // Forward pass: compute the positive score using a dot product helper
    double pos_score = sigmoid(dot_product(v_o, v_c));

    // Accumulate weighted negative contributions for updating the center embedding
    std::vector<double> neg_sum(v_c.size(), 0.0);

    for (int neg_index : negatives) {
        auto v_n = W2.get_row(neg_index);
        double score = sigmoid(dot_product(v_n, v_c));

        // Accumulate contributions to the cost
        for (size_t i = 0; i < v_n.size(); i++) {
            neg_sum[i] += score * v_n[i];
        }

        // Update the negative sample row in W2
        for (size_t i = 0; i < v_n.size(); i++) {
            v_n[i] += learning_rate * (-score) * v_c[i];
        }
    }

    // Update the positive context row in W2
    for (size_t i = 0; i < v_o.size(); i++) {
        v_o[i] += learning_rate * (1 - pos_score) * v_c[i];
    }

    // Update the center row in W1 using the aggregated negative contributions
    for (size_t i = 0; i < v_c.size(); i++) {
        v_c[i] += learning_rate * (1 - pos_score) * v_o[i] - neg_sum[i];
    }
}

Matrix SkipGram::get_embeddings() const { return W1_T; }

std::vector<SkipGram::TrainingPair> SkipGram::generate_pairs(const std::vector<int>& random_walk,
                                                             int window_size) {
    int walk_length = random_walk.size();
    if (walk_length < (2 * window_size + 1))
        throw std::logic_error("Random walks are not long enough for given context window size");
    int num_pairs = window_size * (2 * walk_length - window_size - 1);

    std::vector<TrainingPair> pairs;
    pairs.reserve(num_pairs);

    for (int center_idx = 0; center_idx < walk_length; center_idx++) {
        int left_end = std::max(center_idx - window_size, 0);
        int right_end = std::min(center_idx + window_size, walk_length - 1);

        int center_node = random_walk[center_idx];

        for (int context_idx = left_end; context_idx <= right_end; context_idx++) {
            if (context_idx == center_idx) continue;  // make no pair for the center node
            pairs.emplace_back(center_node, random_walk[context_idx]);
        }
    }

    return pairs;
}

double SkipGram::sigmoid(double val) { return 1 / (1 + exp(-val)); }

double SkipGram::dot_product(std::span<const double> a, std::span<const double> b) {
    if (a.size() != b.size()) throw std::runtime_error("Vectors must be of equal length.");
    return std::inner_product(a.begin(), a.end(), b.begin(), 0.0);
}

double SkipGram::calculate_learning_rate_decrease(double learning_rate, int context_window,
                                                  int walk_length, int total_num_walks) {
    int total_num_training_pairs =
        total_num_walks * (context_window * (2 * walk_length - context_window - 1));
    double learning_rate_decrease = learning_rate / total_num_training_pairs;

    return learning_rate_decrease;
}