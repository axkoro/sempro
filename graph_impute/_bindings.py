from typing import List, Union

from . import cpp_graph_module as g
from . import cpp_imputation_module as im
from . import cpp_louvain_module as l


def load_cpp_graph(
    edges_file: str, features_file: str, feature_type: type
) -> Union[g.GraphFloat, g.GraphInt, g.GraphBool]:
    """
    Load a C++ Graph instance based on the feature type.

    Parameters
    ----------
    edges_file : str
        Path to the file containing the edges.
    features_file : str
        Path to the file containing the features.
    feature_type : type
        The type of features (float, int, or bool).

    Returns
    -------
    Union[g.GraphFloat, g.GraphInt, g.GraphBool]
        The loaded C++ graph instance.

    Raises
    ------
    ValueError
        If an unsupported feature type is provided.
    """
    if feature_type is float:
        return g.GraphFloat(edges_file, features_file)
    elif feature_type is int:
        return g.GraphInt(edges_file, features_file)
    elif feature_type is bool:
        return g.GraphBool(edges_file, features_file)
    else:
        raise ValueError(f"Unsupported feature type: {feature_type}")


_knn_imputer_map = {
    g.GraphFloat: im.KNNImputerFloat,
    g.GraphInt: im.KNNImputerInt,
    g.GraphBool: im.KNNImputerBool,
}


def cpp_create_knn_imputer(
    cpp_graph: Union[g.GraphFloat, g.GraphInt, g.GraphBool], k: int, use_k_hop: bool
) -> Union[im.KNNImputerFloat, im.KNNImputerInt, im.KNNImputerBool]:
    """
    Create a C++ KNN imputer instance for the given graph.

    Parameters
    ----------
    cpp_graph : Union[g.GraphFloat, g.GraphInt, g.GraphBool]
        A C++ graph instance.

    Returns
    -------
    Union[im.KNNImputerFloat, im.KNNImputerInt, im.KNNImputerBool]
        A C++ KNN imputer instance.
    """
    imputer_class = _knn_imputer_map.get(type(cpp_graph))
    return imputer_class(cpp_graph, k, use_k_hop)


_community_imputer_map = {
    g.GraphFloat: im.CommunityImputerFloat,
    g.GraphInt: im.CommunityImputerInt,
    g.GraphBool: im.CommunityImputerBool,
}


def cpp_create_community_imputer(
    cpp_graph: Union[g.GraphFloat, g.GraphInt, g.GraphBool], communities: List[int]
) -> Union[im.CommunityImputerFloat, im.CommunityImputerInt, im.CommunityImputerBool]:
    """
    Create a C++ community imputer instance for the given graph.

    Parameters
    ----------
    cpp_graph : Union[g.GraphFloat, g.GraphInt, g.GraphBool]
        A C++ graph instance.
    communities : List[int]
        A list of community identifiers.

    Returns
    -------
    Union[im.CommunityImputerFloat, im.CommunityImputerInt, im.CommunityImputerBool]
        A C++ community imputer instance.
    """
    imputer_class = _community_imputer_map.get(type(cpp_graph))
    return imputer_class(cpp_graph, communities)


_deepwalk_imputer_map = {
    g.GraphFloat: im.DeepWalkImputerFloat,
    g.GraphInt: im.DeepWalkImputerInt,
    g.GraphBool: im.DeepWalkImputerBool,
}


def cpp_create_deepwalk_imputer(
    cpp_graph: Union[g.GraphFloat, g.GraphInt, g.GraphBool], config: im.DeepWalkConfig
) -> Union[im.DeepWalkImputerFloat, im.DeepWalkImputerInt, im.DeepWalkImputerBool]:
    """
    Create a C++ DeepWalk imputer instance for the given graph.

    Parameters
    ----------
    cpp_graph : Union[g.GraphFloat, g.GraphInt, g.GraphBool]
        A C++ graph instance.
    config : im.DeepWalkConfig
        Configuration for DeepWalk.

    Returns
    -------
    Union[im.DeepWalkImputerFloat, im.DeepWalkImputerInt, im.DeepWalkImputerBool]
        A C++ DeepWalk imputer instance.
    """
    imputer_class = _deepwalk_imputer_map.get(type(cpp_graph))
    return imputer_class(cpp_graph, config)


def cpp_create_deepwalk_config(
    no_edge_weights: bool,
    fusion_coefficient: float,
    walk_length: int,
    num_walks: int,
    embedding_size: int,
    context_window: int,
    num_negative_samples: int,
    smoothing_exponent: float,
    num_epochs: int,
    learning_rate: float,
    top_similar: int,
) -> im.DeepWalkConfig:
    """
    Create and configure a DeepWalkConfig instance.

    Parameters
    ----------
    no_edge_weights : bool
        Whether to perform the edge weight preprocessing step.
    fusion_coefficient : float
        Fusion coefficient.
    walk_length : int
        Length of each random walk.
    num_walks : int
        Number of walks per node.
    embedding_size : int
        Size of the embedding.
    context_window : int
        Size of the context window.
    num_negative_samples : int
        Number of negative samples.
    smoothing_exponent : float
        Smoothing exponent.
    num_epochs : int
        Number of training epochs.
    learning_rate : float
        Learning rate.
    top_similar : int
        Number of most similar nodes (according to DeepWalk embeddings) to be used for imputation.

    Returns
    -------
    im.DeepWalkConfig
        A configured DeepWalkConfig instance.

    Raises
    ------
    Exception
        If the configuration is invalid.
    """
    config = im.DeepWalkConfig()

    config.no_edge_weights = no_edge_weights
    config.fusion_coefficient = fusion_coefficient
    config.walk_length = walk_length
    config.num_walks = num_walks
    config.embedding_size = embedding_size
    config.context_window = context_window
    config.num_negative_samples = num_negative_samples
    config.smoothing_exponent = smoothing_exponent
    config.num_epochs = num_epochs
    config.learning_rate = learning_rate
    config.top_similar = top_similar

    config.validate()

    return config


def cpp_create_louvain_community_detector(
    cpp_graph: g.Graph, louvain_config: l.LouvainConfig
) -> l.Louvain:
    """
    Create a C++ Louvain community detector for the given graph.

    Parameters
    ----------
    cpp_graph : g.Graph
        A C++ graph instance.

    Returns
    -------
    l.Louvain
        A C++ Louvain community detector instance.
    """
    return l.Louvain(cpp_graph, louvain_config)


def cpp_create_louvain_config(
    max_levels: int, max_iterations: int, tolerance: float
) -> l.LouvainConfig:
    config = l.LouvainConfig()

    config.max_levels = max_levels
    config.max_iterations = max_iterations
    config.tolerance = tolerance

    config.validate()
    return config
