#include "DenseVector.hpp"

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

PYBIND11_MAKE_OPAQUE(std::vector<double>);

namespace py = pybind11;

PYBIND11_MODULE(_lin_alg, m) {
    py::bind_vector<std::vector<double> >(m, "VecDouble");
    py::class_<DenseVector>(m, "DenseVector")
        .def(py::init<std::vector<double> &>())
        .def(py::self * float())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("print", &DenseVector::print);
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: DenseVector

        .. autosummary::
           :toctree: _generate

           print()
    )pbdoc";
}