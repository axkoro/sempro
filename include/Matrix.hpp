#pragma once

#include <vector>

#include "Vector.hpp"

// Simple implementation of a matrix stored in row-major format
class Matrix {
   public:
    Matrix(size_t rows, size_t cols, double value = 0.0);

    size_t num_rows() const;
    size_t num_cols() const;

    double& operator()(size_t row, size_t col);
    const double& operator()(size_t row, size_t col) const;

    Vector get_row(size_t row_index) const;

    void add_to_row(const Vector& vec, size_t row_idx);
};