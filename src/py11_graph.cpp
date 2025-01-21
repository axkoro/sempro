#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "Graph.hpp"
#include "GraphBool.hpp"
#include "GraphDouble.hpp"
#include "GraphInt.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_graph, m) {
    // Abstract Graph
    py::class_<Graph, std::shared_ptr<Graph>>(m, "Graph")
        // No constructor for abstract class
        .def("get_num_nodes", &Graph::get_num_nodes)
        .def("get_num_features", &Graph::get_num_features)
        .def("get_label", &Graph::get_label)
        .def("get_missing_features", &Graph::get_missing_features, py::arg("node"))
        .def("is_missing", &Graph::is_missing, py::arg("node"), py::arg("feature"))
        .def("get_neighbours", py::overload_cast<int>(&Graph::get_neighbours, py::const_),
             py::arg("node"))
        .def("get_neighbours", py::overload_cast<int, int>(&Graph::get_neighbours, py::const_),
             py::arg("node"), py::arg("depth"))
        .def("get_degree", &Graph::get_degree, py::arg("node"))
        .def("has_edge", &Graph::has_edge, py::arg("source"), py::arg("target"))
        .def("is_valid_node", &Graph::is_valid_node, py::arg("node"))
        .def("read_edges", &Graph::read_edges, py::arg("edges_path"))
        .def("set_missing", &Graph::set_missing, py::arg("node"), py::arg("feature"),
             py::arg("value"))
        .def("print_edges", &Graph::print_edges)
        .def("print_features_to_file", &Graph::print_features_to_file, py::arg("output_path"));

    // GraphInt
    py::class_<GraphInt, Graph, std::shared_ptr<GraphInt>>(m, "GraphInt")
        .def(py::init<std::string, std::string>(), py::arg("edges_path"), py::arg("features_path"))
        .def("get_int_feature", &GraphInt::get_int_feature, py::arg("node"), py::arg("feature"))
        .def("set_int_feature", &GraphInt::set_int_feature, py::arg("node"), py::arg("feature"),
             py::arg("value"))
        .def("read_features", &GraphInt::read_features, py::arg("features_path"))
        .def("print_features", &GraphInt::print_features);

    // GraphDouble
    py::class_<GraphDouble, Graph, std::shared_ptr<GraphDouble>>(m, "GraphDouble")
        .def(py::init<std::string, std::string>(), py::arg("edges_path"), py::arg("features_path"))
        .def("get_double_feature", &GraphDouble::get_double_feature, py::arg("node"),
             py::arg("feature"))
        .def("set_double_feature", &GraphDouble::set_double_feature, py::arg("node"),
             py::arg("feature"), py::arg("value"))
        .def("read_features", &GraphDouble::read_features, py::arg("features_path"))
        .def("print_features", &GraphDouble::print_features);

    // GraphBool
    py::class_<GraphBool, Graph, std::shared_ptr<GraphBool>>(m, "GraphBool")
        .def(py::init<std::string, std::string>(), py::arg("edges_path"), py::arg("features_path"))
        .def("get_bool_feature", &GraphBool::get_bool_feature, py::arg("node"), py::arg("feature"))
        .def("set_bool_feature", &GraphBool::set_bool_feature, py::arg("node"), py::arg("feature"),
             py::arg("value"))
        .def("read_features", &GraphBool::read_features, py::arg("features_path"))
        .def("print_features", &GraphBool::print_features);
}