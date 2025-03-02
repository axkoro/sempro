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

    RandomWalkGenerator rw_gen(this->graph, edge_weights, config.walk_length, config.num_walks,
                               seed);
    std::vector<std::vector<int>> walks = rw_gen.generate_walks();

    SkipGramConfig cfg(config);
    SkipGram skip_gram(this->graph.get_num_nodes(), cfg, seed);
    skip_gram.train(walks);
    const Matrix& embeddings = skip_gram.get_embeddings();

    impute_features(embeddings, config.top_similar);
}

template <typename T>
void DeepWalkImputer<T>::impute_features(const Matrix& embeddings, int top_similar) {
    int num_nodes = this->graph.get_num_nodes();
    int num_features = this->graph.get_num_features();

    for (int node = 0; node < num_nodes; ++node) {
        for (int feature = 0; feature < num_features; ++feature) {
            if (this->graph.is_missing(node, feature)) {
                // Calculate the similarity ranking vector
                std::vector<std::pair<int, double>> similarity_ranking;
                for (int other_node = 0; other_node < num_nodes; ++other_node) {
                    if (node != other_node) {
                        double similarity =
                            calculate_similarity(embeddings.get_row(node), embeddings.get_row(other_node));
                        similarity_ranking.push_back({other_node, similarity});
                    }
                }

                // Sort the similarity ranking
                std::sort(similarity_ranking.begin(), similarity_ranking.end(),
                          [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
                              return a.second > b.second;
                          });

                // Impute the missing feature using the top similar nodes
                double imputed_value = 0.0;
                int count = 0;
                for (const auto& [similar_node, similarity] : similarity_ranking) {
                    if (!this->graph.is_missing(similar_node, feature)) {
                        imputed_value += this->graph.get_feature(similar_node, feature);
                        count++;
                        if (count == top_similar) break;  // Use the top n similar nodes
                    }
                }
                if (count > 0) {
                    T rounded_average = round_value<T>(imputed_value /= count);
                    this->graph.set_feature(node, feature, imputed_value);
                }
            }
        }
    }
}

template <typename T>
double DeepWalkImputer<T>::calculate_similarity(const Vector& vec1, const Vector& vec2) {
    // Implement your similarity calculation here (e.g., cosine similarity)
    double dot_product = vec1 * vec2;
    return dot_product;
