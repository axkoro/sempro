#pragma once

#include <vector>

class Vector {
   public:
    Vector(size_t size, double value = 0.0);
    Vector(std::vector<double>);

    size_t size() const;

    double& operator[](size_t idx);
    const double& operator[](size_t idx) const;

    // Dot product
    friend double operator*(const Vector& lhs, const Vector& rhs);

    // Scalar multiplication
    friend Vector operator*(const Vector& vec, double scalar);
    friend Vector operator*(double scalar, const Vector& vec);

    // Vector addition and subtraction
    friend Vector operator+(const Vector& a, const Vector& b);
    friend Vector operator-(const Vector& a, const Vector& b);

    // In-place addition
    Vector& operator+=(const Vector& other);

   private:
    size_t size;
    std::vector<double> elements;
};