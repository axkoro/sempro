#pragma once

#include "Imputer.hpp"

// TODO: add documentation for how to use default config (and only change specific paramters)
struct DeepWalkConfig {  // TODO: research best initial parameters
    double fusion_coefficient = 0.5;
    int walk_length = 40;
    int num_walks = 10;
    int embedding_size = 128;
    int context_window = 10;

    bool validate() {
        // TODO: validate parameters
    }
};

class DeepWalkImputer : public Imputer {
   private:
    DeepWalkConfig config;

   public:
    explicit DeepWalkImputer(Graph& g) : Imputer(g), config() {};
    explicit DeepWalkImputer(Graph& g, DeepWalkConfig& c) : Imputer(g), config(c) {};

    void run();

   private:
    void impute_features(const std::vector<std::vector<double>>& embeddings);
};