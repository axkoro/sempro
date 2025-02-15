#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "CommunityImputer.hpp"
#include "DeepWalkImputer.hpp"
#include "KNNImputer.hpp"

namespace py = pybind11;

template <typename T>
void bind_knn_imputer(py::module &m, const std::string &name) {
    py::class_<KNNImputer<T>>(m, name.c_str())
        .def(py::init<AttributedGraph<T> &, int>(), py::arg("graph"), py::arg("depth") = 2)
        .def("run", &KNNImputer<T>::run);
}

template <typename T>
void bind_community_imputer(py::module &m, const std::string &name) {
    py::class_<CommunityImputer<T>>(m, name.c_str())
        .def(py::init<AttributedGraph<T> &, const std::vector<int>>(), py::arg("graph"),
             py::arg("communities"))
        .def("run", &CommunityImputer<T>::run);
}

template <typename T>
void bind_deepwalk_imputer(py::module &m, const std::string &name) {
    py::class_<DeepWalkImputer<T>>(m, name.c_str())
        .def(py::init<AttributedGraph<T> &, int>(), py::arg("graph"), py::arg("seed") = -1)
        .def(py::init<AttributedGraph<T> &, const DeepWalkConfig &, int>(), py::arg("graph"),
             py::arg("config"), py::arg("seed") = -1)
        .def("run", &DeepWalkImputer<T>::run);
}

PYBIND11_MODULE(cpp_imputation_module, m) {
    bind_knn_imputer<double>(m, "KNNImputerFloat");
    bind_knn_imputer<int>(m, "KNNImputerInt");
    bind_knn_imputer<bool>(m, "KNNImputerBool");

    bind_community_imputer<double>(m, "CommunityImputerFloat");
    bind_community_imputer<int>(m, "CommunityImputerInt");
    bind_community_imputer<bool>(m, "CommunityImputerBool");

    bind_deepwalk_imputer<double>(m, "DeepWalkImputerFloat");
    bind_deepwalk_imputer<int>(m, "DeepWalkImputerInt");
    bind_deepwalk_imputer<bool>(m, "DeepWalkImputerBool");

    py::class_<DeepWalkConfig>(m, "DeepWalkConfig")
        .def(py::init<>())
        .def_readwrite("fusion_coefficient", &DeepWalkConfig::fusion_coefficient)
        .def_readwrite("walk_length", &DeepWalkConfig::walk_length)
        .def_readwrite("num_walks", &DeepWalkConfig::num_walks)
        .def_readwrite("embedding_size", &DeepWalkConfig::embedding_size)
        .def_readwrite("context_window", &DeepWalkConfig::context_window)
        .def_readwrite("num_negative_samples", &DeepWalkConfig::num_negative_samples)
        .def_readwrite("smoothing_exponent", &DeepWalkConfig::smoothing_exponent)
        .def_readwrite("num_epochs", &DeepWalkConfig::num_epochs)
        .def_readwrite("learning_rate", &DeepWalkConfig::learning_rate)
        .def("validate", &DeepWalkConfig::validate, "Validate configuration parameters");
}