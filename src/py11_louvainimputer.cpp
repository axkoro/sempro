#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "Graph.hpp"
#include "KNNImputer.hpp"
#include "LouvainImputer.hpp"

namespace py = pybind11;


PYBIND11_MODULE(_louvainimputer, m) {
    py::class_<LouvainImputer>(m, "LouvainImputer")
        .def(py::init<Graph&, std::vector<int>&>(), py::keep_alive<1, 2>())
        .def("compute_community_average", &LouvainImputer::compute_community_average)
        .def("run", &LouvainImputer::run);
        
}