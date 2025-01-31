#pragma once

#include <vector>

class SkipGram {
   private:
    int embedding_size;

   public:
    SkipGram(int embedding_size);

    void train(const std::vector<std::vector<int>>& walks, int context_window);
    std::vector<std::vector<double>>
    get_embeddings();  // TODO: potential optimization: return using move semantics (benchmark this
                       // before changing!!)
};