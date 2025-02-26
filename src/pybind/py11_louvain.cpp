#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <Louvain.hpp>

namespace py = pybind11;

PYBIND11_MODULE(cpp_louvain_module, m) {
    py::class_<Louvain>(m, "Louvain")
        .def(py::init<const Graph&, const LouvainConfig&>(), py::arg("g"), py::arg("config"))
        .def("execute", &Louvain::execute)
        .def("get_modularity", &Louvain::get_modularity);

    py::class_<LouvainConfig>(m, "LouvainConfig")
        .def(py::init<>())
        .def_readwrite("max_levels", &LouvainConfig::max_levels)
        .def_readwrite("max_iterations", &LouvainConfig::max_iterations)
        .def_readwrite("tolerance", &LouvainConfig::tolerance)
        .def("validate", &LouvainConfig::validate, "Validate configuration parameters");
}