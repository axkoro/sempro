#pragma once

// TODO: add documentation for how to use default config (and only change specific paramters)
struct DeepWalkConfig {  // TODO: research best initial parameters
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

/**
 * @brief Configuration parameters for the SkipGram model.
 *
 * TODO: research optimal default values.
 */
struct SkipGramConfig {
    int embedding_size = 128;       ///< Dimensionality of the embeddings.
    int context_window = 10;        ///< Size of the context window.
    int num_negative_samples = 10;  ///< Number of negative samples per training pair.
    double smoothing_exponent =
        0.75;  ///< Exponent for smoothing the sampling distribution (for the negative samples).
    int num_epochs = 5;            ///< Number of training epochs.
    double learning_rate = 0.025;  ///< Initial learning rate.

    /// @brief Default constructor that uses the default member initializers.
    SkipGramConfig() = default;

    /**
     * @brief Constructs a Config from a DeepWalkImputer configuration.
     * @param c A reference to a DeepWalkImputer::Config instance.
     */
    SkipGramConfig(DeepWalkConfig& c)
        : embedding_size(c.embedding_size),
          context_window(c.context_window),
          num_negative_samples(c.num_negative_samples),
          smoothing_exponent(c.smoothing_exponent),
          num_epochs(c.num_epochs),
          learning_rate(c.learning_rate) {}

    /**
     * @brief Validates the configuration parameters.
     * @return true if all parameters are valid, false otherwise.
     */
    bool validate() {
        if (embedding_size <= 0) return false;
        if (context_window <= 0) return false;
        if (num_negative_samples < 0) return false;
        if (smoothing_exponent <= 0) return false;
        if (num_epochs <= 0) return false;
        if (learning_rate <= 0) return false;
        // if (walk_length < (2 * context_window + 1)) return false;
        return true;
    }
};