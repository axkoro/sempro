#pragma once

#include "EdgeWeightCalculator.hpp"
#include "RandomWalkGenerator.hpp"
#include "SkipGram.hpp"

template <typename T>
DeepWalkImputer<T>::DeepWalkImputer(Graph& g, int seed) : Imputer(g), config(), seed(seed){};

template <typename T>
DeepWalkImputer<T>::DeepWalkImputer(Graph& g, const Config& c, int seed)
    : Imputer(g), config(c), seed(seed){};

template <typename T>
void DeepWalkImputer<T>::run() {
    EdgeWeightCalculator<T> ew_calc(graph, config.fusion_coefficient);
    GraphEdgeWeights edge_weights = ew_calc.generate_edge_weights();

    RandomWalkGenerator rw_gen(graph, edge_weights, config.walk_length, config.num_walks, seed);
    std::vector<std::vector<int>> walks = rw_gen.generate_walks();

    SkipGram::Config cfg(config);
    SkipGram skip_gram(graph.get_num_nodes(), cfg, seed);
    skip_gram.train(walks);
    const Matrix& embeddings = skip_gram.get_embeddings();

    impute_features(embeddings);
}

template <typename T>
void DeepWalkImputer<T>::impute_features(const Matrix& embeddings) {}
