#pragma once

#include "Imputer.hpp"

#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test

template <typename T>
class KNNImputer : public Imputer<T> {
    FRIEND_TEST(KNNTest, testGlobalAverage);

   private:
    int k;
    bool use_k_hop;

   public:
    KNNImputer(AttributedGraph<T>& g, int k = 100, bool use_k_hop = false)
        : Imputer<T>(g), k(k), use_k_hop(use_k_hop) {}

    void run() override;
};

#include "KNNImputer.tpp"