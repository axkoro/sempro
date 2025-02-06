#include <vector>

// Simple implementation of a matrix stored in row-major format
class Matrix {
   public:
    Matrix(size_t rows, size_t cols);

    double& operator()(size_t row, size_t col);
    const double& operator()(size_t row, size_t col) const;

    std::vector<double> get_row(size_t row_index) const;

    // Matrix-vector multiplication
    std::vector<double> operator*(const std::vector<double>& vec) const;

    // Compute dot product between a vector and a specified row
    double dot_with_row(const std::vector<double>& vec, size_t row_index) const;
};