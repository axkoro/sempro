#include <iostream>

#include "DenseVector.hpp"

int main(int, char**){
    DenseVector v{1,2,3,4};
    auto u = v * 3;
    u.print();
    bool same = (u == v);
    std::cout << "Same: " << same << "\n";
}
