#include "Matrix.hpp"

#include <stdexcept>

Matrix::Matrix(size_t num_rows, size_t num_cols, double value)
    : num_rows_(num_rows), num_cols_(num_cols), data(num_rows * num_cols, value) {}

size_t Matrix::num_rows() const { return num_rows_; }

size_t Matrix::num_cols() const { return num_cols_; }

double& Matrix::operator()(size_t row, size_t col) {
    size_t idx = row * num_cols_ + col;
    return data[idx];
}

const double& Matrix::operator()(size_t row, size_t col) const {
    size_t idx = row * num_cols_ + col;
    return data[idx];
}

Vector Matrix::get_row(size_t row_idx) const {
    Vector row(num_cols_);
    size_t row_start = row_idx * num_cols_;
    for (size_t i = 0; i < num_cols_; i++) {
        row[i] = data[row_start + i];
    }
    return row;
}

void Matrix::add_to_row(const Vector& vec, size_t row_idx) {
    if (vec.size() != num_cols_)
        throw std::runtime_error("Can't add Vector to Matrix row: Vector doesn't match row length");

    size_t row_start = row_idx * num_cols_;
    for (size_t i = 0; i < num_cols_; i++) {
        data[row_start + i] += vec[i];
    }
}
