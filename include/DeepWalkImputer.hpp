#pragma once

#include "Imputer.hpp"
#include "Matrix.hpp"

template <typename T>
class DeepWalkImputer : public Imputer<T> {
   public:
    // TODO: add documentation for how to use default config (and only change specific paramters)
    struct Config {  // TODO: research best initial parameters
        double fusion_coefficient = 0.5;

        int walk_length = 40;
        int num_walks = 10;

        int embedding_size = 128;
        int context_window = 10;
        int num_negative_samples = 10;
        double smoothing_exponent = 0.75;

        int num_epochs = 5;
        double learning_rate = 0.025;

        bool validate() {  // TODO:
            if (embedding_size <= 0) return false;
            if (context_window <= 0) return false;
            if (num_negative_samples < 0) return false;
            if (smoothing_exponent <= 0) return false;
            if (num_epochs <= 0) return false;
            if (learning_rate <= 0) return false;
            if (walk_length < (2 * context_window + 1))
                return false;  // the full context window would exceed the walk
            return true;
        }
    };

    DeepWalkImputer(Graph& g, int seed = -1);
    DeepWalkImputer(Graph& g, const Config& c, int seed = -1);

    void run();

   private:
    Config& config;
    int seed;

    void impute_features(const Matrix& embeddings);
};

#include "DeepWalkImputer.tpp"