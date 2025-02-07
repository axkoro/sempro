#include "SkipGram.hpp"

#include "Matrix.hpp"

SkipGram::SkipGram(int num_nodes, int embedding_size)
    : num_nodes(num_nodes),
      embedding_size(embedding_size),
      W1_T(num_nodes, embedding_size),
      W2(num_nodes, embedding_size) {}

void SkipGram::train(const std::vector<std::vector<int>>& walks, int context_window) {
    int num_negative_samples = 10;
    int num_epochs = 5;
    double learning_rate = 0.025;  // TODO: add decaying learning rate (see word2vec paper)

    for (int epoch = 0; epoch < num_epochs; epoch++) {
        // TODO: shuffle walks

        for (auto&& walk : walks) {
            std::vector<TrainingPair> pairs = generate_pairs(walk, context_window);

            for (auto&& pair : pairs) {
                std::vector<int> negatives =
                    sample_negative_nodes(pair.center, num_negative_samples);

                // forward pass
                std::vector<double> v_o = W1_T.get_row(pair.center);

                int pos_idx = pair.context;
                std::vector<double> v_c = W2.get_row(pos_idx);
                double pos_score = sigmoid(v_c * v_o);

                std::vector<double> neg_scores(num_negative_samples);
                std::vector<std::vector<double>> v_ns(num_negative_samples,
                                                      std::vector<double>(embedding_size));
                for (int idx : negatives) {
                    std::vector<double> v_n = W2.get_row(idx);
                    double score = sigmoid(v_n * v_o);
                    neg_scores.push_back(score);
                    v_ns.push_back(v_n);
                }

                // backpropagation
                // updates to W2
                std::vector<double> v_c_gradient = (1 - pos_score) * v_c;
                W2.add_to_row(learning_rate * v_c_gradient, pos_idx);

                for (int i = 0; i < num_negative_samples; i++) {
                    std::vector<double> v_n_gradient = -neg_scores[i] * v_c;
                    W2.add_to_row(learning_rate * v_n_gradient, negatives[i]);
                }

                // update to W1
                double neg_sum = 0;
                for (int i = 0; i < num_negative_samples; i++) {
                    neg_sum += neg_scores[i] * v_ns[i];
                }

                std::vector<double> v_o_gradient = (1 - pos_score) * v_o - neg_sum;
                W1.add_to_row(learning_rate * v_o_gradient, pos_idx);
            }
        }
    }
}

std::vector<std::vector<double>> SkipGram::get_embeddings() {
    // potential optimization: return using move semantics (benchmark this before changing!!)
    return std::vector<std::vector<double>>();
}

std::vector<SkipGram::TrainingPair> SkipGram::generate_pairs(const std::vector<int>& random_walk,
                                                             int window_size) {
    int walk_length = random_walk.size();
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