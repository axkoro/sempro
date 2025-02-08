#include "Vector.hpp"

#include <stdexcept>

Vector::Vector(size_t size, double value) : elements(size, value) {}

Vector::Vector(std::vector<double> vec) : elements(vec) {}

inline size_t Vector::size() const { return elements.size(); }

inline double& Vector::operator[](size_t idx) { return elements[idx]; }

inline const double& Vector::operator[](size_t idx) const { return elements[idx]; }

Vector& Vector::operator+=(const Vector& other) {
    size_t size = elements.size();
    if (other.size() != size) throw std::runtime_error("Can't sum Vectors of different sizes");

    for (size_t i = 0; i < size; i++) {
        elements[i] += other[i];
    }

    return *this;
}

Vector& Vector::operator-=(const Vector& other) {
    size_t size = elements.size();
    if (other.size() != size) throw std::runtime_error("Can't subtract Vectors of different sizes");

    for (size_t i = 0; i < size; i++) {
        elements[i] -= other[i];
    }

    return *this;
}

double operator*(const Vector& lhs, const Vector& rhs) {
    size_t vec_size = lhs.size();
    if (rhs.size() != vec_size)
        throw std::runtime_error("Can't perform dot product with Vectors of different sizes");

    double result = 0.0;
    for (size_t i = 0; i < vec_size; i++) {
        result += lhs[i] * rhs[i];
    }

    return result;
}

Vector operator*(const Vector& vec, double scalar) { return operator*(scalar, vec); }

Vector operator*(double scalar, const Vector& vec) {
    size_t size = vec.size();
    Vector result(size);
    for (size_t i = 0; i < size; i++) {
        result[i] = scalar * vec[i];
    }
    return result;
}

Vector operator+(const Vector& a, const Vector& b) {
    Vector result(a);
    result += b;
    return result;
}

Vector operator-(const Vector& a, const Vector& b) {
    Vector result(a);
    result -= b;
    return result;
}
