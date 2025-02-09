#include "SkipGram.hpp"

#include <algorithm>
#include <cmath>

SkipGram::SkipGram(int num_nodes, DeepWalkImputer::DeepWalkConfig& config, int seed)
    : num_nodes(num_nodes), config(config) {
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

    NegativeSampler sampler(walks, num_nodes, config.smoothing_exponent);
    std::random_device rd;
    std::mt19937 rng(rd());

    for (int epoch = 0; epoch < config.num_epochs; epoch++) {
        std::vector<size_t> walk_indices(walks.size());
        std::iota(walk_indices.begin(), walk_indices.end(), 0);
        std::shuffle(walk_indices.begin(), walk_indices.end(), rng);

        for (size_t idx : walk_indices) {
            std::vector<TrainingPair> pairs = generate_pairs(walks[idx], config.context_window);
            std::shuffle(pairs.begin(), pairs.end(), rng);

            for (auto&& pair : pairs) {
                process_pair(pair, learning_rate, sampler);
                learning_rate -= learning_rate_decrease;
            }
        }
    }
}

void SkipGram::process_pair(TrainingPair pair, double learning_rate, NegativeSampler& sampler) {
    std::vector<int> negatives =
        sampler.sample_negative_nodes(pair.center, config.num_negative_samples);

    // forward pass
    Vector v_c = W1_T.get_row(pair.center);

    Vector v_o = W2.get_row(pair.context);
    double pos_score = sigmoid(v_o * v_c);

    std::vector<double> neg_scores(config.num_negative_samples);
    std::vector<Vector> v_n_list(config.num_negative_samples, Vector(config.embedding_size));
    for (int i = 0; i < config.num_negative_samples; i++) {
        int neg_index = negatives[i];
        Vector v_n = W2.get_row(neg_index);
        double score = sigmoid(v_n * v_c);
        neg_scores[i] = score;
        v_n_list[i] = v_n;
    }

    // backpropagation
    // updates to W2
    Vector gradient_v_o = (1 - pos_score) * v_c;
    W2.add_to_row(learning_rate * gradient_v_o, pair.context);

    for (int i = 0; i < config.num_negative_samples; i++) {
        Vector gradient_v_n = -neg_scores[i] * v_c;
        W2.add_to_row(learning_rate * gradient_v_n, negatives[i]);
    }

    // update to W1
    Vector neg_sum(config.embedding_size, 0.0);
    for (int i = 0; i < config.num_negative_samples; i++) {
        neg_sum += neg_scores[i] * v_n_list[i];
    }

    Vector gradient_v_c = (1 - pos_score) * v_o - neg_sum;
    W1_T.add_to_row(learning_rate * gradient_v_c, pair.center);
}

Matrix SkipGram::get_embeddings() {
    // potential optimization: return using move semantics (benchmark this before changing!!)
    return W1_T;
}

std::vector<SkipGram::TrainingPair> SkipGram::generate_pairs(const std::vector<int>& random_walk,
                                                             int window_size) {
    int walk_length = random_walk.size();
    if (walk_length < (2 * window_size + 1))
        throw std::logic_error("Random walks are not long enough for given context window size");
    int num_pairs = window_size * (2 * walk_length - window_size - 1);
    std::vector<TrainingPair> pairs(num_pairs);

    int insert_idx = 0;
    for (int center_idx = 0; center_idx < walk_length; center_idx++) {
        int left_end = std::max(center_idx - window_size, 0);
        int right_end = std::min(center_idx + window_size, walk_length - 1);
        for (int target_idx = left_end; target_idx <= right_end; target_idx++) {
            if (target_idx == center_idx) continue;  // make no pair for the center node
            pairs[insert_idx].center = random_walk[center_idx];
            pairs[insert_idx].context = random_walk[target_idx];
            insert_idx++;
        }
    }

    return pairs;
}

double SkipGram::sigmoid(double val) { return 1 / (1 + exp(-val)); }

double SkipGram::calculate_learning_rate_decrease(double learning_rate, int context_window,
                                                  int walk_length, int total_num_walks) {
    int total_num_training_pairs =
        total_num_walks * (context_window * (2 * walk_length - context_window - 1));
    double learning_rate_decrease = learning_rate / total_num_training_pairs;

    return learning_rate_decrease;
}