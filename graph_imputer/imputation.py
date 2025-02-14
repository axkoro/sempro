from abc import ABC, abstractmethod

from .bindings import (
    cpp_create_community_imputer,
    cpp_create_deepwalk_config,
    cpp_create_deepwalk_imputer,
    cpp_create_knn_imputer,
    cpp_create_louvain_community_detector,
)
from .graph import Graph


class Imputer(ABC):
    def __init__(self, graph: Graph):
        self.graph = graph
        self.cpp_imputer = None

    @abstractmethod
    def impute(self) -> None:
        """
        Impute missing node features and return a new Graph with imputed data.
        """
        pass


class KNNImputer(Imputer):
    def __init__(self, graph: Graph, depth: int = 2):
        super().__init__(graph)
        self.cpp_imputer = cpp_create_knn_imputer(graph.cpp_graph, depth)

    def impute(self) -> None:
        self.cpp_imputer.run()


class CommunityImputer(Imputer):
    def __init__(self, graph: Graph, community_algorithm: str = "louvain"):
        super().__init__(graph)
        self.community_algorithm = community_algorithm
        self.communities_ready = False

    def find_communities(self):
        if self.community_algorithm == "louvain":
            community_detector = cpp_create_louvain_community_detector(self.graph.cpp_graph)
        else:
            raise NotImplementedError(
                f"'{self.community_algorithm}' is not a valid community detection algorithm"
            )
        communities = community_detector.execute()
        self.cpp_imputer = cpp_create_community_imputer(self.graph.cpp_graph, communities)

    def impute(self) -> None:
        if not self.communities_ready:
            self.find_communities()
        self.cpp_imputer.run()


class DeepWalkImputer(Imputer):
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

    def impute(self) -> None:
        self.cpp_imputer.run()


def create_imputer(strategy: str, graph: Graph, **kwargs) -> Imputer:
    strategy = strategy.lower()
    if strategy == "knn":
        return KNNImputer(graph, **kwargs)
    elif strategy == "community":
        return CommunityImputer(graph, **kwargs)
    elif strategy == "deepwalk":
        return DeepWalkImputer(graph, **kwargs)
    else:
        raise ValueError(f"Unknown strategy: {strategy}")
