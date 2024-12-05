#include <pybind11/pybind11.h>

#include "Example.hpp"
namespace py = pybind11;

PYBIND11_MODULE(_example, m) {
    py::class_<Example>(m, "Example")
        .def(py::init<std::string>())
        .def("get_string", &Example::get_string);
}