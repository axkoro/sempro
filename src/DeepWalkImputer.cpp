#include "DeepWalkImputer.hpp"

#include "EdgeWeightCalculator.hpp"
#include "RandomWalkGenerator.hpp"
#include "SkipGram.hpp"

void DeepWalkImputer::run() {
    EdgeWeightCalculator ew_calc(graph, config.fusion_coefficient);
    StaticMinimalGraph weighted_graph = ew_calc.generate_weighted_graph();

    RandomWalkGenerator rw_gen(weighted_graph, config.walk_length, config.num_walks);
    std::vector<std::vector<int>> walks = rw_gen.generate_walks();

    SkipGram skip_gram(graph.get_num_nodes(), config.embedding_size);
    skip_gram.train(walks, config.context_window);
    Matrix embeddings = skip_gram.get_embeddings();

    impute_features(embeddings);
}

void DeepWalkImputer::impute_features(const Matrix& embeddings) {}
