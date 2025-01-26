#include <pybind11/pybind11.h>
#include <pybind11/stl.h> 

#include "Imputer.hpp"
#include "KNNImputer.hpp"
#include "LouvainImputer.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_strats, m) {
    py::class_<Imputer, std::shared_ptr<Imputer>>(m, "Imputer").def("run", &Imputer::run);

    py::class_<KNNImputer, Imputer, std::shared_ptr<KNNImputer>>(m, "KNNImputer")
        .def(py::init<GraphBool&>(), py::arg("graph"))
        .def(py::init<GraphDouble&>(), py::arg("graph"))
        .def(py::init<GraphInt&>(), py::arg("graph"))
        .def("run", &KNNImputer::run)
        .def("set_depth", &KNNImputer::set_depth, py::arg("k"))
        .def("get_depth", &KNNImputer::get_depth);

    py::class_<LouvainImputer>(m, "LouvainImputer")
        .def(py::init<GraphBool&, const std::vector<int>&>(), py::keep_alive<1, 2>())
        .def(py::init<GraphDouble&, const std::vector<int>&>(), py::keep_alive<1, 2>())
        .def(py::init<GraphInt&, const std::vector<int>&>(), py::keep_alive<1, 2>())
        .def("run", &LouvainImputer::run);
}