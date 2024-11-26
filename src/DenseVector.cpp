#include "DenseVector.hpp"

#include <iostream>


DenseVector::DenseVector(std::initializer_list<double> init) : data(init) {}

DenseVector::DenseVector(std::vector<double> &init) : data{init} {}

DenseVector DenseVector::operator*(double scale) const {
	DenseVector ret = *this;
	for (auto& x : ret.data)
		x *= scale;
	return ret;
}

bool DenseVector::operator==(const DenseVector &other) const {
	if(other.data.size() != this->data.size()) return false;
	
	bool same = true;
	for (int i = 0; i < other.data.size(); ++i) {
		same = same && (this->data[i] == other.data[i]);
	}
	return same;
}

bool DenseVector::operator!=(const DenseVector &other) const {
        return !(*this == other);
}

void DenseVector::print() const {
    for (auto x : data)
        std::cout << x << ", ";
    std::cout << "\n";
}