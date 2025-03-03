#pragma once

#include "Configs.hpp"
#include "Imputer.hpp"
#include "Matrix.hpp"

template <typename T>
class DeepWalkImputer : public Imputer<T> {
   public:
    DeepWalkImputer(AttributedGraph<T>& g, int seed = -1);
    DeepWalkImputer(AttributedGraph<T>& g, const DeepWalkConfig& c, int seed = -1);

    void run();

   private:
    DeepWalkConfig config;
    int seed;

    void impute_features(const Matrix& embeddings);
    std::vector<std::pair<int, double>> get_similarity_ranking(int node, const Matrix& embeddings);
    double calculate_similarity(const Vector& vec1, const Vector& vec2);
};

#include "DeepWalkImputer.tpp"