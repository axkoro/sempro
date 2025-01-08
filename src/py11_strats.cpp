#include <pybind11/pybind11.h>

#include "Imputer.hpp"
#include "KNNImputer.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_imputer, m) {
    py::class_<Imputer, std::shared_ptr<Imputer>>(m, "Imputer").def("run", &Imputer::run);

    py::class_<KNNImputer, Imputer, std::shared_ptr<KNNImputer>>(m, "KNNImputer")
        .def(py::init<GraphBool&>(), py::arg("graph"))
        .def(py::init<GraphDouble&>(), py::arg("graph"))
        .def(py::init<GraphInt&>(), py::arg("graph"))
        .def("run", &KNNImputer::run)
        .def("set_depth", &KNNImputer::set_depth, py::arg("k"))
        .def("get_depth", &KNNImputer::get_depth);

    m.def("compute_global_average_bool", &compute_global_average_bool, py::arg("graph"),
          py::arg("feature"));
    m.def("compute_global_average_double", &compute_global_average_double, py::arg("graph"),
          py::arg("feature"));
    m.def("compute_global_average_int", &compute_global_average_int, py::arg("graph"),
          py::arg("feature"));
    m.def("to_bool", &to_bool, py::arg("value"));
    m.def("to_int", &to_int, py::arg("value"));
}