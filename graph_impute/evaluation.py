import os
import tempfile
from typing import List, Optional, Type, Union

import numpy as np
import numpy.typing as npt
from sklearn.metrics import r2_score

from .graph import Graph


def evaluate_imputed_graph(
    graph: Graph,
    input_features_path: str,
    reference_path: str,
    train_neural: bool = False,
    dataset_name: Optional[str] = None,
) -> dict:
    """
    Evaluate imputed graph features against a reference.

    Parameters
    ----------
    graph : Graph
        Graph object containing imputed features.
    reference_path : str
        Path to the reference features file used for evaluation.
    """
    temp_file = tempfile.NamedTemporaryFile(delete=False, mode="w", suffix=".txt")
    graph.save(temp_file.name)

    results = evaluate_imputed_features_file(
        imputed_features_path=temp_file.name,
        input_features_path=input_features_path,
        reference_path=reference_path,
        feature_type=graph.feature_type,
        train_neural=train_neural,
        dataset_name=dataset_name,
    )

    temp_file.close()
    try:
        os.remove(temp_file.name)
    except OSError:
        print("Failed to delete temp files for evaluation")
        pass

    return results


def evaluate_imputed_features_file(
    imputed_features_path: str,
    reference_path: str,
    input_features_path: str,
    feature_type: Union[Type[float], Type[int], Type[bool]] = float,
    train_neural: bool = False,
    dataset_name: Optional[str] = None,
) -> dict:
    """
    Evaluate the quality of imputed features against a reference.

    Parameters
    ----------
    imputed_features_path : str
        File path to the imputed features.
    reference_path : str
        File path to the reference features.
    input_features_path : str
        File path to the input features that indicate missing values.
    feature_type : {float, int, bool}, optional
        Data type of the features, by default float.

    Returns
    -------
    tuple of float
        A tuple containing:
        - overlap_total (float): Ratio of overlapping features over total features.
        - overlap_missing (float): Ratio of overlapping features over missing features.
        - max_error (float): Maximum error between imputed and reference features.
        - avg_error (float): Average error between imputed and reference features.
        - r2_score (float): R² score measuring the imputation performance.

    Raises
    ------
    ValueError
        If the dimensions of the imputed and reference features do not match, or if there are
        no missing features in `input_features_path` to evaluate.
    """
    imputed_features = _read_features(imputed_features_path, feature_type)
    ref_features = _read_features(reference_path, feature_type)

    if np.shape(imputed_features) != np.shape(ref_features):
        raise ValueError(
            "Reference feature dimensions don't match imputed feature dimensions "
            "(either num_features or num_nodes)"
        )
    total_num_features = imputed_features.shape[0] * imputed_features.shape[1]

    num_overlapping = _compute_overlap(imputed_features, ref_features)
    overlap_total = num_overlapping / total_num_features

    num_missing, missing_map = _count_missing_features(input_features_path)
    if num_missing > 0:
        overlap_missing = (num_overlapping - total_num_features + num_missing) / num_missing
    else:
        raise ValueError(
            f"There are no missing features in '{input_features_path}', so nothing to evaluate."
        )

    max_error, avg_error = _measure_avg_and_max_distance(imputed_features, ref_features)

    r2_score = _measure_r2_score(ref_features, imputed_features, missing_map)

    results = {
        "overlap_total": overlap_total,
        "overlap_missing": overlap_missing,
        "max_error": max_error,
        "avg_error": avg_error,
        "r2_score": r2_score,
    }

    if train_neural:
        from ._evaluation_train import train_and_evaluate_gnn_with_imputation

        model_accuracy_original, model_accuracy_imputed = train_and_evaluate_gnn_with_imputation(
            dataset_name, imputed_features
        )

        results["model_accuracy_original"] = model_accuracy_original
        results["model_accuracy_imputed"] = model_accuracy_imputed

    return results


def _read_features(
    features_path: str, feature_type: Union[Type[float], Type[int], Type[bool]] = float
) -> npt.NDArray:
    with open(features_path, "rb") as f:
        lines = f.readlines()
    num_lines = len(lines)
    num_features = len(lines[0].split(b"\t")[1].split(b" "))

    features = np.zeros((num_lines, num_features), dtype=feature_type)
    for line_idx, line in enumerate(lines):
        line_features_str = line.split(b"\t")[1]
        if b"#" in line_features_str:
            raise ValueError(f"Missing feature detected on line {line_idx}.")
        line_values = np.fromstring(line_features_str, sep=",", dtype=np.float64)
        features[line_idx, :] = line_values
    return features


def _compute_overlap(
    features_x: npt.NDArray, features_y: npt.NDArray, tolerance: float = 1e-5
) -> int:
    ground_truth = np.isclose(features_x, features_y)
    return ground_truth.sum()


def _count_missing_features(features_path: str) -> tuple[int, npt.NDArray]:
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
                    missing_feature_indices.append((line_idx, feature_idx))
                feature_idx = feature_idx + 1
            line_idx = line_idx + 1

    missing_feature_indices = np.array(missing_feature_indices)
    return num_missing_features, missing_feature_indices


def _measure_avg_and_max_distance(
    features_x: npt.NDArray, features_y: npt.NDArray
) -> tuple[float, float]:
    if features_x.dtype == np.bool_:
        diff = np.abs(features_x ^ features_y)
    else:
        diff = np.abs(features_x - features_y)
    return np.max(diff), np.mean(diff)


def _measure_r2_score(
    ref_features: npt.NDArray,
    pred_features: npt.NDArray,
    missing_map: List[tuple[int, int]],
    all_features: bool = False,
) -> float:
    if all_features:
        score = r2_score(ref_features, pred_features)
    else:  # compute r2 score only over the missing features / predictions
        feature_type = ref_features.dtype
        x = np.zeros(len(missing_map), dtype=feature_type)
        y = np.zeros(len(missing_map), dtype=feature_type)

        missing_map = np.array(missing_map)
        x = ref_features[missing_map[:, 0], missing_map[:, 1]]
        y = pred_features[missing_map[:, 0], missing_map[:, 1]]

        score = r2_score(x, y)

    return score
