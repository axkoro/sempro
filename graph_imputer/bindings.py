from typing import Union

from . import cpp_graph_module as g
from . import cpp_imputation_module as im


def load_cpp_graph(
    edges_file: str, features_file: str, feature_type: type
) -> Union[g.GraphFloat, g.GraphInt, g.GraphBool]:
    """
    Loads a C++ Graph instance based on the feature type.
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
    cpp_graph: Union[g.GraphFloat, g.GraphInt, g.GraphBool], depth: int
) -> Union[im.KNNImputerFloat, im.KNNImputerInt, im.KNNImputerBool]:
    imputer_class = _knn_imputer_map.get(type(cpp_graph))
    return imputer_class(cpp_graph)


_community_imputer_map = {
    g.GraphFloat: im.CommunityImputerFloat,
    g.GraphInt: im.CommunityImputerInt,
    g.GraphBool: im.CommunityImputerBool,
}


def cpp_create_community_imputer(
    cpp_graph: Union[g.GraphFloat, g.GraphInt, g.GraphBool],
) -> Union[im.CommunityImputerFloat, im.CommunityImputerInt, im.CommunityImputerBool]:
    imputer_class = _community_imputer_map.get(type(cpp_graph))
    return imputer_class(cpp_graph)


_deepwalk_imputer_map = {
    g.GraphFloat: im.DeepWalkImputerFloat,
    g.GraphInt: im.DeepWalkImputerInt,
    g.GraphBool: im.DeepWalkImputerBool,
}


def cpp_create_deepwalk_imputer(
    cpp_graph: Union[g.GraphFloat, g.GraphInt, g.GraphBool],
) -> Union[im.DeepWalkImputerFloat, im.DeepWalkImputerInt, im.DeepWalkImputerBool]:
    imputer_class = _deepwalk_imputer_map.get(type(cpp_graph))
    return imputer_class(cpp_graph)


def cpp_create_deepwalk_config(
    fusion_coefficient: float,
    walk_length: int,
    num_walks: int,
    embedding_size: int,
    context_window: int,
    num_negative_samples: int,
    smoothing_exponent: float,
    num_epochs: int,
    learning_rate: float,
) -> im.DeepWalkConfig:
    config = im.DeepWalkConfig()

    config.fusion_coefficient = fusion_coefficient
    config.walk_length = walk_length
    config.num_walks = num_walks
    config.embedding_size = embedding_size
    config.context_window = context_window
    config.num_negative_samples = num_negative_samples
    config.smoothing_exponent = smoothing_exponent
    config.num_epochs = num_epochs
    config.learning_rate = learning_rate

    config.validate()

    return config
