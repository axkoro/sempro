#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(_example, m) {
    py::class_<Example>(m, "Example")
        .def(py::init<std::string>())
        .def("getString", &Example::getString);
}