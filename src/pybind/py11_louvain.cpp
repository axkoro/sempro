#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <Louvain.hpp>

namespace py = pybind11;

PYBIND11_MODULE(cpp_louvain_module, m) {
    py::class_<Louvain>(m, "Louvain")
        .def(py::init<const Graph&, const LouvainConfig&>(), py::arg("g"), py::arg("config"))
        .def("execute", &Louvain::execute)
        .def("get_modularity", &Louvain::get_modularity);
}