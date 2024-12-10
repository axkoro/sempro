#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Graph.hpp"

namespace py = pybind11;

PYBIND11_MODULE(py11_graph, m) {
    py::class_<Graph>(m, "Graph")
        .def(py::init<>()) // Default constructor
        .def(py::init<std::vector<int>&, std::vector<int>&>(), py::arg("offsets"), py::arg("edges")) // Offset-Edge constructor
        .def(py::init<std::string, std::string>(), py::arg("edges_path"), py::arg("features_path")) // File constructor

        // Accessors
        .def("get_num_nodes", &Graph::get_num_nodes)
        .def("get_num_features", &Graph::get_num_features)
        .def("get_offsets", &Graph::get_offsets)
        .def("get_edges", &Graph::get_edges)
        .def("get_features", &Graph::get_features, py::arg("node"))
        .def("get_missing_features", &Graph::get_missing_features, py::arg("node"))

        // Graph operations
        .def("get_neighbours", py::overload_cast<int>(&Graph::get_neighbours, py::const_), py::arg("node"))
        .def("get_neighbours", py::overload_cast<int, int>(&Graph::get_neighbours, py::const_), py::arg("node"), py::arg("depth"))
        .def("get_degree", &Graph::get_degree, py::arg("node"))
        .def("has_edge", &Graph::has_edge, py::arg("source"), py::arg("target"))
        .def("is_valid_node", &Graph::is_valid_node, py::arg("node"))

        // Utility methods
        .def("print_edges", &Graph::print_edges)
        .def("print_features", &Graph::print_features);
}