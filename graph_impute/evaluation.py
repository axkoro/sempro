import os
import tempfile
from typing import List, Type, Union

import numpy as np
import numpy.typing as npt
from sklearn.metrics import r2_score

from .graph import Graph


def evaluate_imputed_graph(graph: Graph, reference_path: str):
    temp_file = tempfile.NamedTemporaryFile(delete=False, mode="w", suffix=".txt")
    Graph.save(temp_file)

    evaluate_imputed_features_file(temp_file, reference_path)

    temp_file.close()
    try:
        os.remove()
    except OSError:
        pass


def evaluate_imputed_features_file(
    imputed_features_path: str,
    reference_path: str,
    input_features_path: str,
    feature_type: Union[Type[float], Type[int], Type[bool]] = float,
):
    imputed_features = read_features(imputed_features_path, feature_type)
    ref_features = read_features(reference_path, feature_type)

    if np.shape(imputed_features) != np.shape(ref_features):
        raise ValueError(
            "Reference feature dimensions don't match imputed feature dimensions (either num_features or num_nodes)"
        )
    num_features = imputed_features.shape[0] * imputed_features.shape[1]

    absolute_overlap = compute_overlap(imputed_features, ref_features)
    overlap_total = absolute_overlap / num_features

    num_missing, missing_map = count_missing_features(input_features_path)
    if num_missing > 0:
        overlap_missing = (absolute_overlap - num_features + num_missing) / num_missing
    else:
        raise ValueError(
            f"There are no missing features in '{input_features_path}', so nothing to evaluate."
        )

    max_error, avg_error = measure_avg_and_max_distance(imputed_features, ref_features)

    r2_score = measure_r2_score(ref_features, imputed_features, missing_map)

    # TODO: train neural network

    return overlap_total, overlap_missing, max_error, avg_error, r2_score


def read_features(
    features_path: str, feature_type: Union[Type[float], Type[int], Type[bool]] = float
) -> npt.NDArray:
    num_lines = sum(1 for _ in open(features_path, "rb"))
    with open(features_path, "rb") as f:
        line = f.readline()
        num_features = len(line.split(b"\t")[1].split(b" "))
    orig_features = np.zeros((num_lines, num_features), feature_type)

    with open(features_path, "rb") as f:
        line_idx = 0
        for line in f:
            features = line.split(b"\t")[1].split(b", ")
            for feature_idx in range(num_features):
                # TODO: throw if non_number in imputed_features_path
                orig_features[line_idx, feature_idx] = feature_type(features[feature_idx])
            line_idx += 1
    return orig_features


def compute_overlap(
    features_x: npt.NDArray, features_y: npt.NDArray, tolerance: float = 1e-5
) -> int:
    ground_truth = np.isclose(features_x, features_y)
    return ground_truth.sum()


def count_missing_features(features_path: str) -> tuple[int, List[tuple[int, int]]]:
    num_missing_features = 0
    missing_feature_indices = []
    with open(features_path, encoding="UTF-8") as f:
        line_idx = 0
        for line in f:
            features = line.split("\t")[1].split(", ")
            feature_idx = 0
            for item in features:
                if str(item) == "#":
                    num_missing_features += 1
                    missing_feature_indices.append(
                        (line_idx, feature_idx)
                    )  # deviation from original measure-quality.py script (indented here)
                feature_idx = feature_idx + 1
            line_idx = line_idx + 1
    return num_missing_features, missing_feature_indices


def measure_avg_and_max_distance(
    features_x: npt.NDArray, features_y: npt.NDArray
) -> tuple[float, float]:
    if features_x.dtype == np.bool:
        diff = np.abs(features_x ^ features_y)
    else:
        diff = np.abs(features_x - features_y)
    return np.max(diff), np.mean(diff)


def measure_r2_score(
    features_x: npt.NDArray, features_y: npt.NDArray, missing_map: List[tuple[int, int]]
) -> float:
    x = np.zeros(len(missing_map))
    y = np.zeros(len(missing_map))

    for i in range(len(missing_map)):
        x[i] = features_x[missing_map[i][0], missing_map[i][1]]
        y[i] = features_y[missing_map[i][0], missing_map[i][1]]

    report = r2_score(x, y)
    return report
