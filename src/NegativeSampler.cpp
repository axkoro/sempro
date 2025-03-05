#include "NegativeSampler.hpp"

#include <algorithm>
#include <cmath>

NegativeSampler::NegativeSampler(const std::vector<std::vector<int>> &random_walks, int num_nodes,
                                 double smoothing_exponent, int seed)
    : index_distribution(0, num_nodes - 1), probability_distribution(0.0, 1.0) {
    if (seed == -1) {
        std::random_device rd;
        rng.seed(rd());
    } else {
        rng.seed(seed);
    }

    std::vector<int> node_occurrences(num_nodes, 0);
    for (const auto &walk : random_walks) {
        for (int node : walk) {
            node_occurrences[node]++;
        }
    }

    // calculated (smoothed) relative frequencies
    std::vector<double> relative_frequencies(num_nodes);
    double total = 0.0;
    for (int i = 0; i < num_nodes; i++) {
        double smoothed_frequency = std::pow(node_occurrences[i], smoothing_exponent);
        relative_frequencies[i] = smoothed_frequency;
        total += smoothed_frequency;
    }
    for (int i = 0; i < num_nodes; i++) {
        relative_frequencies[i] /= total;
    }

    alias_table = build_alias_table(relative_frequencies);
}

int NegativeSampler::draw_sample() {
    int selected_index = index_distribution(rng);
    double random_prob = probability_distribution(rng);

    if (random_prob < alias_table.probabilities[selected_index]) return selected_index;
    return alias_table.aliases[selected_index];
}

std::vector<int> NegativeSampler::sample_negative_nodes(int center_node, int num_samples) {
    std::vector<int> samples;
    samples.reserve(num_samples);

    const size_t batch_factor = 1;  // TODO: maybe adjust based on frequency of context_node
    size_t max_batch_size = (num_samples * batch_factor);
    std::vector<int> batch_indices(max_batch_size);
    std::vector<double> batch_probs(max_batch_size);

    while (samples.size() < static_cast<size_t>(num_samples)) {
        size_t needed = num_samples - samples.size();
        size_t batch_size = std::min(max_batch_size, needed * batch_factor);

        std::generate(batch_indices.begin(), batch_indices.begin() + batch_size,
                      [&]() { return index_distribution(rng); });
        std::generate(batch_probs.begin(), batch_probs.begin() + batch_size,
                      [&]() { return probability_distribution(rng); });

        for (size_t i = 0; i < batch_size; ++i) {
            int sample = (batch_probs[i] < alias_table.probabilities[batch_indices[i]])
                             ? batch_indices[i]
                             : alias_table.aliases[batch_indices[i]];
            if (sample == center_node) continue;
            samples.push_back(sample);
            if (samples.size() >= static_cast<size_t>(num_samples)) break;
        }
    }

    return samples;
}

// Vose's algorithm for building an alias table (numerically stable version), see Repo-Wiki
NegativeSampler::AliasTable NegativeSampler::build_alias_table(
    std::vector<double> discrete_distribution) {
    size_t num_events = discrete_distribution.size();
    for (size_t i = 0; i < num_events; i++) {
        discrete_distribution[i] *= num_events;
    }

    std::vector<int> overfull_indices;
    std::vector<int> underfull_indices;
    for (size_t i = 0; i < num_events; i++) {
        if (discrete_distribution[i] >= 1) {
            overfull_indices.push_back(i);
        } else {
            underfull_indices.push_back(i);
        }
    }

    AliasTable table = {
        std::vector<int>(num_events),  // aliases
        discrete_distribution          // probabilities
    };

    while (!underfull_indices.empty() && !overfull_indices.empty()) {
        int underfull = underfull_indices.back();
        underfull_indices.pop_back();
        int overfull = overfull_indices.back();
        overfull_indices.pop_back();

        table.aliases[underfull] = overfull;

        double updated_large_prob =
            (table.probabilities[overfull] + table.probabilities[underfull]) - 1;
        table.probabilities[overfull] = updated_large_prob;

        if (updated_large_prob < 1) {
            underfull_indices.push_back(overfull);
        } else {
            overfull_indices.push_back(overfull);
        }
    }

    while (!overfull_indices.empty()) {
        table.probabilities[overfull_indices.back()] = 1.0;
        overfull_indices.pop_back();
    }
    while (!underfull_indices.empty()) {  // possible due to numerical instability
        table.probabilities[underfull_indices.back()] = 1.0;
        underfull_indices.pop_back();
    }

    return table;
}
