from typing import Type, Union

from bindings import load_cpp_graph

from . import cpp_graph_module as g


class Graph:
    def __init__(
        self,
        cpp_graph: Union[
            g.GraphFloat,
            g.GraphInt,
            g.GraphBool,
        ],
    ):
        """
        Wraps a given cpp_graph object.
        """
        if cpp_graph.isinstance(g.GraphFloat):
            self.feature_type = float
        elif cpp_graph.isinstance(g.GraphInt):
            self.feature_type = int
        elif cpp_graph.isinstance(g.GraphBool):
            self.feature_type = bool
        else:
            raise TypeError(f"Unsupported Graph object: {type(cpp_graph).__name__}")

        self.cpp_graph = cpp_graph

    @classmethod
    def load(
        cls,
        edges_file: str,
        features_file: str,
        feature_type: Union[Type[float], Type[int], Type[bool]] = float,
    ):
        """
        Loads a graph from the given edge and feature files.
        The underlying C++ implementation is selected based on feature_type.
        """
        feature_type = feature_type
        cpp_graph = load_cpp_graph(edges_file, features_file, feature_type)
        return cls(cpp_graph)

    def save(self, output_file: str) -> None:
        """Saves the graph features to a file."""
        self.cpp_graph.print_features_to_file(output_file)
