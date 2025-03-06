#pragma once

#include "DeepWalkImputer.hpp"  // because IntelliSense is stupid
#include "EdgeWeightCalculator.hpp"
#include "RandomWalkGenerator.hpp"
#include "SkipGram.hpp"

template <typename T>
DeepWalkImputer<T>::DeepWalkImputer(AttributedGraph<T>& g, int seed)
    : Imputer<T>(g), config(), seed(seed){};

template <typename T>
DeepWalkImputer<T>::DeepWalkImputer(AttributedGraph<T>& g, const DeepWalkConfig& c, int seed)
    : Imputer<T>(g), config(c), seed(seed){};

template <typename T>
void DeepWalkImputer<T>::run() {
    EdgeWeightCalculator<T> ew_calc(this->graph, config.fusion_coefficient);
    GraphEdgeWeights edge_weights = ew_calc.generate_edge_weights();
    // GraphEdgeWeights edge_weights(this->graph); // sets every edge weight to 1

    RandomWalkGenerator rw_gen(this->graph, edge_weights, config.walk_length, config.num_walks,
                               seed);
    std::vector<std::vector<int>> walks = rw_gen.generate_walks();

    SkipGramConfig cfg(config);
    SkipGram skip_gram(this->graph.get_num_nodes(), cfg, seed);
    skip_gram.train(walks);
    const Matrix& embeddings = skip_gram.get_embeddings();

    impute_features(embeddings);
}

template <typename T>
void DeepWalkImputer<T>::impute_features(const Matrix& embeddings) {
    int num_nodes = this->graph.get_num_nodes();
    int num_features = this->graph.get_num_features();

    for (int node = 0; node < num_nodes; ++node) {
        auto missing_feature_indices = this->graph.get_missing_indices(node);
        if (missing_feature_indices.empty()) continue;

        std::vector<std::pair<int, double>> similarity_ranking =
            get_similarity_ranking(node, embeddings);

        for (int feature_idx : missing_feature_indices) {
            double feature_sum = 0.0;  // for average calculation
            int count = 0;
            for (const auto& [similar_node, similarity] : similarity_ranking) {
                if (this->graph.is_missing(similar_node, feature_idx)) continue;

                feature_sum += this->graph.get_feature(similar_node, feature_idx);
                count++;
                if (count == config.top_similar) break;  // Use the top n similar nodes
            }
            if (count > 0) {
                T imputed_val = round_value<T>(feature_sum / static_cast<double>(count));
                this->graph.set_feature(node, feature_idx, imputed_val);
            } else {
                this->graph.set_feature(node, feature_idx, 1.0);
            }
            this->graph.set_missing(node, feature_idx, false);
        }
    }
}

/**
 * @return A std::vector of std::pair<int, double>, where pair.first is the index of the compared
 * node and pair.second is the similarity value of this node and the input node that was passed. The
 * vector is sorted by the similarity value.
 */
template <typename T>
std::vector<std::pair<int, double>> DeepWalkImputer<T>::get_similarity_ranking(
    int node, const Matrix& embeddings) {
    int num_nodes = this->graph.get_num_nodes();

    std::vector<std::pair<int, double>> similarity_ranking;
    similarity_ranking.reserve(num_nodes - 1);

    for (int other_node = 0; other_node < num_nodes; ++other_node) {
        if (node == other_node) continue;

        double similarity =
            calculate_similarity(embeddings.get_row(node), embeddings.get_row(other_node));

        similarity_ranking.emplace_back(other_node, similarity);
    }

    std::sort(similarity_ranking.begin(), similarity_ranking.end(),
              [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
                  return a.second > b.second;
              });

    return similarity_ranking;
}

template <typename T>
double DeepWalkImputer<T>::calculate_similarity(std::span<const double> vec1,
                                                std::span<const double> vec2) {
    return SkipGram::dot_product(vec1, vec2);
}