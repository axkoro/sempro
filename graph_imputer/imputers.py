from abc import ABC, abstractmethod

from bindings import (
    cpp_create_community_imputer,
    cpp_create_deepwalk_config,
    cpp_create_deepwalk_imputer,
    cpp_create_knn_imputer,
)
from graph import Graph


class BaseImputer(ABC):
    def __init__(self, graph: Graph):
        self.graph = graph
        self.cpp_imputer = None

    @abstractmethod
    def impute(self) -> Graph:
        """
        Impute missing node features and return a new Graph with imputed data.
        """
        pass


class KNNImputer(BaseImputer):
    def __init__(self, graph: Graph, depth: int = 2):
        super().__init__(graph)
        self.cpp_imputer = cpp_create_knn_imputer(graph.cpp_graph, depth)

    def impute(self) -> Graph:
        imputed_cpp_graph = self.cpp_imputer.run()
        return Graph(imputed_cpp_graph)


class CommunityImputer(BaseImputer):
    def __init__(self, graph: Graph):
        super().__init__(graph)
        self.cpp_imputer = cpp_create_community_imputer(graph.cpp_graph)

    def impute(self) -> Graph:
        imputed_cpp_graph = self.cpp_imputer.run()
        return Graph(imputed_cpp_graph)


class DeepWalkImputer(BaseImputer):
    def __init__(
        self,
        graph: Graph,
        fusion_coefficient: float = 0.6,
        walk_length: int = 40,
        num_walks: int = 10,
        embedding_size: int = 128,
        context_window: int = 10,
        num_negative_samples: int = 10,
        smoothing_exponent: float = 0.75,
        num_epochs: int = 5,
        learning_rate: float = 0.025,
    ):
        super().__init__(graph)
        self.config = cpp_create_deepwalk_config(
            fusion_coefficient,
            walk_length,
            num_walks,
            embedding_size,
            context_window,
            num_negative_samples,
            smoothing_exponent,
            num_epochs,
            learning_rate,
        )
        self.cpp_imputer = cpp_create_deepwalk_imputer(graph.cpp_graph, self.config)

    def impute(self) -> Graph:
        imputed_cpp_graph = self.cpp_imputer.run()
        return Graph(imputed_cpp_graph)


def create_imputer(strategy: str, graph: Graph) -> BaseImputer:
    strategy = strategy.lower()
    if strategy == "knn":
        return KNNImputer(graph)
    elif strategy == "community":
        return CommunityImputer(graph)
    elif strategy == "deepwalk":
        return DeepWalkImputer(graph)
    else:
        raise ValueError(f"Unknown strategy: {strategy}")
