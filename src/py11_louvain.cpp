#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <Louvain.hpp>

namespace py = pybind11;

PYBIND11_MODULE(_louvain, m) {
    py::class_<Louvain>(m, "Louvain")
        .def(py::init<Graph&>(), py::arg("graph"))
        .def("execute", &Louvain::execute);
}