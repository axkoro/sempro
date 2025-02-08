#include "SkipGram.hpp"

#include <cmath>

SkipGram::SkipGram(int num_nodes, int embedding_size)
    : num_nodes(num_nodes),
      embedding_size(embedding_size),
      W1_T(num_nodes, embedding_size),
      W2(num_nodes, embedding_size) {}

void SkipGram::train(const std::vector<std::vector<int>>& walks, int context_window) {
    // TODO: configurable parameters
    int num_negative_samples = 10;
    int num_epochs = 5;
    double learning_rate = 0.025;  // TODO: add decaying learning rate (see word2vec paper)

    NodeDistribution distribution(walks);

    for (int epoch = 0; epoch < num_epochs; epoch++) {
        // TODO: shuffle walks

        for (auto&& walk : walks) {
            std::vector<TrainingPair> pairs = generate_pairs(walk, context_window);

            for (auto&& pair : pairs) {  // see repo wiki for thorough explanation
                std::vector<int> negatives =
                    sample_negative_nodes(distribution, pair.center, num_negative_samples);

                // forward pass
                Vector v_c = W1_T.get_row(pair.center);

                Vector v_o = W2.get_row(pair.context);
                double pos_score = sigmoid(v_o * v_c);

                std::vector<double> neg_scores(num_negative_samples);
                std::vector<Vector> v_n_list(num_negative_samples, Vector(embedding_size));
                for (int i = 0; i < num_negative_samples; i++) {
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

                for (int i = 0; i < num_negative_samples; i++) {
                    Vector gradient_v_n = -neg_scores[i] * v_c;
                    W2.add_to_row(learning_rate * gradient_v_n, negatives[i]);
                }

                // update to W1
                Vector neg_sum(embedding_size, 0.0);
                for (int i = 0; i < num_negative_samples; i++) {
                    neg_sum += neg_scores[i] * v_n_list[i];
                }

                Vector gradient_v_c = (1 - pos_score) * v_o - neg_sum;
                W1_T.add_to_row(learning_rate * gradient_v_c, pair.center);
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

double SkipGram::sigmoid(double val) { return 1 / (1 + exp(-val)); }

std::vector<int> SkipGram::sample_negative_nodes(NodeDistribution distribution, int center_node,
                                                 int num_samples) {
    std::vector<int> samples(num_samples);

    int samples_taken = 0;
    while (samples_taken < num_samples) {
        int sample = distribution.draw_sample();
        if (sample == center_node) continue;
        samples[samples_taken] = sample;
        samples_taken++;
    }

    return samples;
}
