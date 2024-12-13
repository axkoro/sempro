#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "Graph.hpp"
#include "Imputer.hpp"
#include "KNNImputer.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_strats, m) {
    py::class_<KNNImputer>(m, "KNNImputer")
        .def(py::init<Graph&>())
        .def("run", &KNNImputer::run)
        .def("set_depth", &KNNImputer::set_depth);
}