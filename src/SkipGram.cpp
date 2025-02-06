#include "SkipGram.hpp"

SkipGram::SkipGram(int embedding_size) {}

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
            pairs[insert_idx].in = random_walk[center_idx];
            pairs[insert_idx].out = random_walk[target_idx];
            insert_idx++;
        }
    }

    return pairs;
}