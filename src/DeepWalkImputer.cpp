#include "DeepWalkImputer.hpp"

#include "EdgeWeightCalculator.hpp"
#include "RandomWalkGenerator.hpp"
#include "SkipGram.hpp"

DeepWalkImputer::DeepWalkImputer(Graph& g, int seed) : Imputer(g), config(), seed(seed) {};
DeepWalkImputer::DeepWalkImputer(Graph& g, Config& c, int seed)
    : Imputer(g), config(c), seed(seed) {};

void DeepWalkImputer::run() {
    EdgeWeightCalculator ew_calc(graph, config.fusion_coefficient);
    StaticMinimalGraph weighted_graph = ew_calc.generate_weighted_graph();

    RandomWalkGenerator rw_gen(weighted_graph, config.walk_length, config.num_walks, seed);
    std::vector<std::vector<int>> walks = rw_gen.generate_walks();

    SkipGram::Config cfg(config);
    SkipGram skip_gram(graph.get_num_nodes(), cfg, seed);
    skip_gram.train(walks);
    const Matrix& embeddings = skip_gram.get_embeddings();

    impute_features(embeddings);
}

void DeepWalkImputer::impute_features(const Matrix& embeddings) {}
