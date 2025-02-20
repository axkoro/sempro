import inspect
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
        self.cpp_imputer = cpp_create_knn_imputer(graph.cpp_graph, k, use_k_hop)

    def impute(self) -> None:
        self.cpp_imputer.run()


class CommunityImputer(Imputer):
    """
    Imputer using community detection.

    Parameters
    ----------
    graph : Graph
        The graph to be imputed.
    community_algorithm : str, optional
        The algorithm used for community detection (default is "louvain").
    """

    def __init__(self, graph: Graph, community_algorithm: str = "louvain"):
        super().__init__(graph)
        self.community_algorithm = community_algorithm
        self.communities_ready = False

    def detect_communities(self):
        """
        Detect communities in the graph using the specified community_algorithm (passed during construction).
        Then instantiate the actual C++ imputer class.

        Raises
        ------
        NotImplementedError
            If the specified community detection algorithm is not implemented.
        """
        if self.community_algorithm == "louvain":
            community_detector = cpp_create_louvain_community_detector(self.graph.cpp_graph)
        else:
            raise NotImplementedError(
                f"'{self.community_algorithm}' is not a valid community detection algorithm"
            )
        communities = community_detector.execute()
        self.cpp_imputer = cpp_create_community_imputer(self.graph.cpp_graph, communities)
        self.communities_ready = True

    def impute(self) -> None:
        """
        Impute missing features using community detection.
        If communities have not been previously detected by runnning detect_communities(),
        this step will be performed before running the actual imputation strategy.

        Returns
        -------
        None
        """
        if not self.communities_ready:
            self.detect_communities()
        self.cpp_imputer.run()


class DeepWalkImputer(Imputer):
    """
    Imputer using DeepWalk embeddings.

    Parameters
    ----------
    graph : Graph
        The graph to be imputed.
    fusion_coefficient : float, optional
        Fusion coefficient (default is 0.6).
    walk_length : int, optional
        Length of each random walk (default is 40).
    num_walks : int, optional
        Number of walks per node (default is 10).
    embedding_size : int, optional
        Size of the embedding (default is 128).
    context_window : int, optional
        Size of the context window (default is 10).
    num_negative_samples : int, optional
        Number of negative samples (default is 10).
    smoothing_exponent : float, optional
        Smoothing exponent (default is 0.75).
    num_epochs : int, optional
        Number of training epochs (default is 5).
    learning_rate : float, optional
        Learning rate for training (default is 0.025).
    """

    def __init__(
        self,
        graph: Graph,
        fusion_coefficient: float = 0.6,
        walk_length: int = 80,
        num_walks: int = 10,
        embedding_size: int = 128,
        context_window: int = 10,
        num_negative_samples: int = 10,
        smoothing_exponent: float = 0.75,
        num_epochs: int = 1,
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
