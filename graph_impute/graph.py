from typing import Type, Union

from . import cpp_graph_module as g
from .bindings import load_cpp_graph


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
        if isinstance(cpp_graph, g.GraphFloat):
            self.feature_type = float
        elif isinstance(cpp_graph, g.GraphInt):
            self.feature_type = int
        elif isinstance(cpp_graph, g.GraphBool):
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
        Load a graph from the specified edge and feature files.

        Parameters
        ----------
        edges_file : str
            Path to the file containing the edges.
        features_file : str
            Path to the file containing the features.
        feature_type : {float, int, bool}, optional
            The type of features to load (default is float).

        Returns
        -------
        Graph
            A Graph instance wrapping the corresponding C++ graph.
        """
        cpp_graph = load_cpp_graph(edges_file, features_file, feature_type)
        return cls(cpp_graph)

    def save(self, output_file: str) -> None:
        """
        Save the graph features to a file.

        Parameters
        ----------
        output_file : str
            Path to the output file.
        """
        self.cpp_graph.print_features_to_file(output_file)
