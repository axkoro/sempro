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
    for (int epoch = 0; epoch < num_epochs; epoch++) {
        // TODO: shuffle walks

        for (auto&& walk : walks) {
            std::vector<TrainingPair> pairs = generate_pairs(walk, context_window);

            for (auto&& pair : pairs) {
                std::vector<int> negatives =
                    sample_negative_nodes(pair.center, num_negative_samples);

                // forward pass
                std::vector<double> v_o = W1_T.get_row(pair.center);

                std::vector<double> scores(num_negative_samples + 1);
                int pos_idx = pair.context;
                scores[0] = sigmoid(W2.dot_with_row(v_o, pos_idx));
                for (int idx : negatives) {
                    double score = sigmoid(W2.dot_with_row(v_o, idx));
                    scores.push_back(score);
                }

                // backpropagation
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