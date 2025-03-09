import inspect
from abc import ABC, abstractmethod

from ._bindings import (
    cpp_create_community_imputer,
    cpp_create_deepwalk_config,
    cpp_create_deepwalk_imputer,
    cpp_create_knn_imputer,
    cpp_create_louvain_community_detector,
    cpp_create_louvain_config,
)
from .graph import Graph


class Imputer(ABC):
    """
    Abstract base class for imputers.

    Parameters
    ----------
    graph : Graph
        The graph to be imputed.
    """

    def __init__(self, graph: Graph):
        self.graph = graph
        self.cpp_imputer = None

    @abstractmethod
    def impute(self) -> None:
        """
        Impute missing node features in-place, meaning the imputed features
        will be stored within the Graph that was passed to the Imputer.

        Returns
        -------
        None
        """
        pass

    @abstractmethod
    def get_config() -> dict:
        """
        Returns
        -------
        Configuration of the Imputer as a dict (paramter_name: str -> parameter_value: Any)
        """
        pass


class KNNImputer(Imputer):
    """
    Imputer using a k-nearest neighbors approach.

    Parameters
    ----------
    graph : Graph
        The graph to be imputed.
    k : int, optional
        The k for the KNN imputer (default is 2).
    """

    def __init__(self, graph: Graph, k: int = 100, use_k_hop: bool = False):
        super().__init__(graph)
        self.k = k
        self.use_k_hop = use_k_hop
        self.cpp_imputer = cpp_create_knn_imputer(graph.cpp_graph, k, use_k_hop)

    def impute(self) -> None:
        self.cpp_imputer.run()

    def get_config(self) -> dict:
        return {"k": self.k, "use_k_hop": self.use_k_hop}


class CommunityImputer(Imputer):
    """
    Imputer using community detection.
    """

    def __init__(
        self,
        graph: Graph,
        community_algorithm: str = "louvain",
        max_levels: int = 10,
        max_iterations: int = 50,
        tolerance: float = 1e-4,
    ):
        super().__init__(graph)
        self.communities_ready = False
        # Store configuration parameters
        self.community_algorithm = community_algorithm
        self.max_levels = max_levels
        self.max_iterations = max_iterations
        self.tolerance = tolerance

        if community_algorithm == "louvain":
            config = cpp_create_louvain_config(max_levels, max_iterations, tolerance)
            self.community_detector = cpp_create_louvain_community_detector(
                self.graph.cpp_graph, config
            )
        else:
            raise NotImplementedError(
                f"'{community_algorithm}' is not a valid community detection algorithm"
            )

    def detect_communities(self):
        communities = self.community_detector.execute()
        self.cpp_imputer = cpp_create_community_imputer(self.graph.cpp_graph, communities)
        self.communities_ready = True

    def impute(self) -> None:
        if not self.communities_ready:
            self.detect_communities()
        self.cpp_imputer.run()

    def get_config(self) -> dict:
        return {
            "community_algorithm": self.community_algorithm,
            "max_levels": self.max_levels,
            "max_iterations": self.max_iterations,
            "tolerance": self.tolerance,
        }


class DeepWalkImputer(Imputer):
    """
    Imputer using DeepWalk embeddings.
    """

    def __init__(
        self,
        graph: Graph,
        no_edge_weights: bool = False,
        fusion_coefficient: float = 0.6,
        walk_length: int = 80,
        num_walks: int = 10,
        embedding_size: int = 128,
        context_window: int = 10,
        num_negative_samples: int = 10,
        smoothing_exponent: float = 0.75,
        num_epochs: int = 1,
        learning_rate: float = 0.025,
        top_similar: int = 10,
        similarity_metric: str = "cosine",
    ):
        super().__init__(graph)
        # Store configuration parameters
        self.no_edge_weights = no_edge_weights
        self.fusion_coefficient = fusion_coefficient
        self.walk_length = walk_length
        self.num_walks = num_walks
        self.embedding_size = embedding_size
        self.context_window = context_window
        self.num_negative_samples = num_negative_samples
        self.smoothing_exponent = smoothing_exponent
        self.num_epochs = num_epochs
        self.learning_rate = learning_rate
        self.top_similar = top_similar
        self.similarity_metric = similarity_metric

        self.config = cpp_create_deepwalk_config(
            no_edge_weights,
            fusion_coefficient,
            walk_length,
            num_walks,
            embedding_size,
            context_window,
            num_negative_samples,
            smoothing_exponent,
            num_epochs,
            learning_rate,
            top_similar,
            similarity_metric,
        )
        self.cpp_imputer = cpp_create_deepwalk_imputer(graph.cpp_graph, self.config)

    def impute(self) -> None:
        self.cpp_imputer.run()

    def get_config(self) -> dict:
        return {
            "no_edge_weights": self.no_edge_weights,
            "fusion_coefficient": self.fusion_coefficient,
            "walk_length": self.walk_length,
            "num_walks": self.num_walks,
            "embedding_size": self.embedding_size,
            "context_window": self.context_window,
            "num_negative_samples": self.num_negative_samples,
            "smoothing_exponent": self.smoothing_exponent,
            "num_epochs": self.num_epochs,
            "learning_rate": self.learning_rate,
            "top_similar": self.top_similar,
        }


def create_imputer(strategy: str, graph: Graph, **kwargs) -> Imputer:
    """
    Factory function to create an imputer based on the provided strategy.

    Parameters
    ----------
    strategy : str
        The imputation strategy to use. Valid options are 'knn', 'community', and 'deepwalk'.
    graph : Graph
        The graph to be imputed.
    **kwargs
        Additional keyword arguments passed to the imputer constructor.
        See documentation of the individual imputers for possible arguments.

        Invalid arguments will be ignored and a warning will be printed.

    Returns
    -------
    Imputer
        An instance of a concrete Imputer.

    Raises
    ------
    ValueError
        If an unknown strategy is provided.
    """
    imputer_classes = {
        "knn": KNNImputer,
        "community": CommunityImputer,
        "deepwalk": DeepWalkImputer,
    }
    try:
        imputer_class = imputer_classes[strategy.lower()]
    except KeyError as e:
        raise ValueError(f"Unknown strategy: '{strategy}'") from e

    valid_params = inspect.signature(imputer_class.__init__).parameters
    filtered_kwargs = {}
    for parameter_name, parameter_value in kwargs.items():
        if parameter_value is None:
            continue
        if parameter_name in valid_params:
            filtered_kwargs[parameter_name] = parameter_value
        else:
            print(
                f"Warning: Ignoring parameter '{parameter_name}' that was passed, but which isn't supported by strategy '{strategy}'"
            )

    return imputer_class(graph, **filtered_kwargs)
