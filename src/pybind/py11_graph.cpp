#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "AttributedGraph.hpp"
#include "Graph.hpp"

namespace py = pybind11;

template <typename T>
void bind_attributed_graph(py::module &m, const std::string &name) {
    py::class_<AttributedGraph<T>, Graph>(m, name.c_str())
        .def(py::init<std::string, std::string>(), py::arg("edges_file"), py::arg("features_file"))
        //    .def("get_num_features", &AttributedGraph<T>::get_num_features)
        //    .def("get_feature", &AttributedGraph<T>::get_feature)
        //    .def("get_label", &AttributedGraph<T>::get_label)
        //    .def("get_missing_features", &AttributedGraph<T>::get_missing_features)
        //    .def("set_feature", &AttributedGraph<T>::set_feature)
        //    .def("set_missing", &AttributedGraph<T>::set_missing)
        //    .def("is_missing", &AttributedGraph<T>::is_missing)
        //    .def("read_features", &AttributedGraph<T>::read_features)
        //    .def("print_features", &AttributedGraph<T>::print_features)
        .def("print_features_to_file", &AttributedGraph<T>::print_features_to_file);
}

PYBIND11_MODULE(cpp_graph_module, m) {
    py::class_<GraphException, std::exception>(m, "GraphException")
        .def(py::init<const std::string &>());

    py::class_<Graph>(m, "Graph")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("edges_path"))
        //   .def(py::init<std::vector<int>, std::vector<int>>(), py::arg("offsets"),
        //   py::arg("edges"))
        .def("get_num_nodes", &Graph::get_num_nodes)
        .def("get_num_edges", &Graph::get_num_edges)
        //    .def("get_neighbours", (std::vector<int>(Graph::*)(int) const) &
        //    Graph::get_neighbours) .def("get_neighbours_depth",
        //         (std::vector<int>(Graph::*)(int, int) const) & Graph::get_neighbours)
        //    .def("get_degree", &Graph::get_degree)
        //    .def("has_edge", &Graph::has_edge)
        //    .def("is_valid_node", &Graph::is_valid_node)
        .def("print_edges", &Graph::print_edges);

    bind_attributed_graph<double>(m, "GraphFloat");
    bind_attributed_graph<int>(m, "GraphInt");
    bind_attributed_graph<bool>(m, "GraphBool");
}