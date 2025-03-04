#include "Matrix.hpp"

#include <stdexcept>

Matrix::Matrix(size_t num_rows, size_t num_cols, double value)
    : num_rows_(num_rows), num_cols_(num_cols), data(num_rows * num_cols, value) {}

Vector Matrix::get_row(size_t row_idx) const {
    size_t row_start = row_idx * num_cols_;
    return Vector(data.begin() + row_start, data.begin() + row_start + num_cols_);
}

void Matrix::add_to_row(const Vector& vec, size_t row_idx) {
    if (vec.size() != num_cols_)
        throw std::runtime_error("Can't add Vector to Matrix row: Vector doesn't match row length");

    size_t row_start = row_idx * num_cols_;
    for (size_t i = 0; i < num_cols_; i++) {
        data[row_start + i] += vec[i];
    }
}
