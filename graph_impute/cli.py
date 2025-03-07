import argparse
import importlib
import time
from copy import deepcopy
from textwrap import dedent
from typing import Any

import yaml

from .evaluation import evaluate_imputed_features_file
from .graph import Graph
from .imputation import create_imputer


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description="Imputation tool with multiple strategies.",
    )

    general = parser.add_argument_group("General Options")
    general.add_argument(
        "--strategy",
        choices=["knn", "community", "deepwalk"],
        required=True,
        help="Imputation strategy to use.",
    )
    general.add_argument(
        "--dataset", type=str, help='Pre-configured dataset name (e.g., "amazon").'
    )
    general.add_argument("--edges", type=str, help="File path for edge data.")
    general.add_argument("--features", type=str, help="File path for feature data.")
    general.add_argument(
        "--output",
        type=str,
        default="./imputed_features.txt",
        help="File path to store imputed features.",
    )
    general.add_argument(
        "--feature-type",
        choices=["bool", "int", "float"],
        default="float",
        help="Type of feature data in the graph.",
    )
    general.add_argument(
        "--config",
        type=str,
        default="./datasets.yaml",
        help="Configuration file mapping dataset names to file paths.",
    )

    eval_group = parser.add_argument_group("Evaluation Options")
    eval_group.add_argument(
        "--evaluate", action="store_true", help="Evaluate the quality of imputed features."
    )
    eval_group.add_argument(
        "--reference", type=str, help="Reference data file path for evaluation."
    )
    eval_group.add_argument("--time", action="store_true", help="Time major processing steps.")
    eval_group.add_argument(
        "--train",
        action="store_true",
        default=None,
        help="Train a neural network using the imputed data to evaluate the quality in downstream tasks.",
    )

    knn_group = parser.add_argument_group("KNN Imputer Options")
    knn_group.add_argument(
        "--k",
        type=int,
        # default=100,
        help="The number of neighbors to be used for KNN imputation. If --use-k-hop was passed: number of hops to use.",
    )
    knn_group.add_argument(
        "--use-k-hop",
        action="store_true",
        default=None,  # because otherwise 'False' is default which makes checking if this argument was passed impossible (for warning messages)
        help="Use neighbors within k hops instead of the k nearest (for KNN imputation).",
    )

    community_group = parser.add_argument_group("Community Imputer Options")
    community_group.add_argument(
        "--community-algorithm",
        type=str,
        # default="louvain",
        help="Algorithm to use for community detection.",
    )

    community_group.add_argument(
        "--max-levels",
        type=int,
        help="The maximum number of coarsening levels for the Louvain method.",
    )

    community_group.add_argument(
        "--max-iterations",
        type=int,
        help="The maximum number of iterations in each coarsening level of the Louvain method.",
    )

    community_group.add_argument(
        "--tolerance",
        type=float,
        help="The minimum improvement in modularity required to continue with the next iteration within the Louvain method.",
    )

    deepwalk_group = parser.add_argument_group("DeepWalk Imputer Options")
    deepwalk_group.add_argument(
        "--no-edge-weights",
        action="store_true",
        default=None,  # because otherwise 'False' is default which makes checking if this argument was passed impossible (for warning messages)
        help="Whether to perform the edge weight preprocessing step.",
    )
    deepwalk_group.add_argument(
        "--fusion-coefficient",
        type=float,
        # default=0.6,
        help="Fusion coefficient for DeepWalk imputation.",
    )
    deepwalk_group.add_argument(
        "--walk-length",
        type=int,
        # default=40,
        help="Length of each random walk.",
    )
    deepwalk_group.add_argument(
        "--num-walks",
        type=int,
        # default=10,
        help="Number of walks per node.",
    )
    deepwalk_group.add_argument(
        "--embedding-size",
        type=int,
        # default=128,
        help="Size of the embedding vector.",
    )
    deepwalk_group.add_argument(
        "--context-window",
        type=int,
        # default=10,
        help="Context window size for embedding learning.",
    )
    deepwalk_group.add_argument(
        "--num-negative-samples",
        type=int,
        # default=10,
        help="Number of negative samples per positive sample.",
    )
    deepwalk_group.add_argument(
        "--smoothing-exponent",
        type=float,
        # default=0.75,
        help="Smoothing exponent for sampling distribution.",
    )
    deepwalk_group.add_argument(
        "--num-epochs",
        type=int,
        # default=5,
        help="Number of epochs for training.",
    )
    deepwalk_group.add_argument(
        "--learning-rate",
        type=float,
        # default=0.025,
        help="Learning rate for the optimizer.",
    )
    deepwalk_group.add_argument(
        "--top-similar",
        type=int,
        help="Number of most similar nodes (according to DeepWalk embeddings) to be used for imputation.",
    )

    return parser.parse_args()


def validate_command(args: argparse.Namespace) -> None:
    """
    Validates the parsed command line arguments.
    Raises:
        ValueError: if validation rules are violated.
    """
    if not args.dataset:
        if not args.edges:
            raise ValueError("Please specify --edges")
        if not args.features:
            raise ValueError("Please specify --features")

    if args.evaluate and not args.dataset and not args.reference:
        raise ValueError("Evaluation requires either a --dataset or a --reference to be specified.")

    if args.dataset:
        if args.edges or args.features:
            print(
                "Warning: When --dataset is provided, file paths for --edges and --features will be overridden by the dataset configuration."
            )

    if args.train:
        if not is_extra_installed("train_evaluate"):
            raise ImportError("Invalid flag: --train. Install extra 'train_evaluate' to use it.")

        valid_datasets = ["amazon", "cora", "twitch", "github", "genius"]
        if not args.dataset or args.dataset not in valid_datasets:
            raise NotImplementedError(
                f"--train currently only works with the following datasets: {', '.join(valid_datasets)}."
            )

    # if args.plot:
    #     if not is_extra_installed("plot"):
    #         raise ImportError("Invalid flag: --plot. Install extra 'plot' to use it.")


def is_extra_installed(extra: str):
    """Check if all dependencies for a given extra are installed."""

    # Should match extras_require in setup.py!
    EXTRA_DEPENDENCIES = {
        "train_evaluate": ["torch", "torch_geometric", "tqdm"],
        "plot": ["matplotlib"],
        "test": ["pytest", "coverage"],
    }

    if extra not in EXTRA_DEPENDENCIES:
        valid_extras = ", ".join(EXTRA_DEPENDENCIES.keys())
        raise ValueError(f"Invalid extra '{extra}. Available options: {valid_extras}.")

    dependencies = EXTRA_DEPENDENCIES[extra]
    return all(importlib.util.find_spec(dep) is not None for dep in dependencies)


def load_dataset_config(args: argparse.Namespace) -> tuple[str, str, str, str]:
    if args.dataset:
        config_path = args.config  # will always be set due to default value
        with open(config_path) as file:
            data = yaml.safe_load(file)
        try:
            feature_type_str = data[args.dataset]["feature_type"]
            edges_path = data[args.dataset]["edges"]
            features_path = data[args.dataset]["features"]
            if args.evaluate:
                reference_path = data[args.dataset]["reference"]
            else:
                reference_path = ""
        except KeyError as err:
            raise LookupError(
                f"Could not find all relevant data for data set '{args.dataset}' in '{config_path}' (see documentation for information about the config)"
            ) from err
    else:
        edges_path = args.edges
        features_path = args.features
        feature_type_str = args.feature_type
        if args.evaluate:
            reference_path = args.reference
        else:
            reference_path = ""

    type_map = {
        "bool": bool,
        "int": int,
        "float": float,
    }
    feature_type = type_map[feature_type_str]

    return edges_path, features_path, reference_path, feature_type


def extract_parameters(args: argparse.Namespace) -> dict[str, Any]:
    non_parameters = [
        "strategy",
        "dataset",
        "edges",
        "features",
        "output",
        "feature_type",
        "config",
        "evaluate",
        "reference",
        "time",
        "plot",
        "train",
    ]
    parameters = deepcopy(args.__dict__)
    for parameter in non_parameters:
        parameters.pop(parameter, None)
    return parameters


def main():
    args = parse_args()
    validate_command(args)
    edges_path, features_path, reference_path, feature_type = load_dataset_config(args)

    load_time_start = time.time()
    graph = Graph.load(edges_path, features_path, feature_type=feature_type)
    kwargs = extract_parameters(args)
    imputer = create_imputer(args.strategy, graph, **kwargs)
    load_time_end = time.time()

    if args.strategy == "community":
        community_detection_time_start = time.time()
        imputer.detect_communities()
        community_detection_time_end = time.time()

    impute_time_start = time.time()
    imputer.impute()
    impute_time_end = time.time()

    save_time_start = time.time()
    graph.save(args.output)
    save_time_end = time.time()

    if args.time:
        load_time = load_time_end - load_time_start
        impute_time = impute_time_end - impute_time_start
        save_time = save_time_end - save_time_start

        times = [load_time, impute_time, save_time]
        formatted_times = [f"{t:.2f}" for t in times]
        max_width = max(len(t_str) for t_str in formatted_times)

        output = dedent(f"""\
            Impute time: {impute_time:{max_width}.2f} s
            Load time:   {load_time:{max_width}.2f} s
            Save time:   {save_time:{max_width}.2f} s""")

        if args.strategy == "community":
            community_detection_time = community_detection_time_end - community_detection_time_start
            output += f"\nCommunity detection time: {community_detection_time:.2f} s"

        print(output)

    if args.evaluate:
        evaluate_time_start = time.time()

        if args.train is None:
            args.train = False

        results = evaluate_imputed_features_file(
            imputed_features_path=args.output,
            reference_path=reference_path,
            input_features_path=features_path,
            feature_type=feature_type,
            train_neural=args.train,
            dataset_name=args.dataset,
        )

        evaluate_time_end = time.time()
        if args.time:
            evaluate_time = evaluate_time_end - evaluate_time_start
            print(f"Evaluate time: {evaluate_time:{max_width}.2f} s\n")

        eval_values = [
            results["overlap_total"] * 100,
            results["overlap_missing"] * 100,
            results["max_error"],
            results["avg_error"],
            results["r2_score"],
        ]
        formatted_eval = [f"{val:.2f}" for val in eval_values]
        eval_max_width = max(len(val_str) for val_str in formatted_eval)

        print(f"overlap (total):   {results['overlap_total'] * 100:{eval_max_width}.2f} %")
        print(f"overlap (missing): {results['overlap_missing'] * 100:{eval_max_width}.2f} %")
        print(f"max error (abs):   {results['max_error']:{eval_max_width}.2f}")
        print(f"avg error (abs):   {results['avg_error']:{eval_max_width}.2f}")
        print(f"R2 score:          {results['r2_score']:{eval_max_width}.2f}")

        if args.train:
            print(
                f"model accuracy (original): {results['model_accuracy_original'] * 100:{eval_max_width}.2f} %"
            )
            print(
                f"model accuracy (imputed): {results['model_accuracy_imputed'] * 100:{eval_max_width}.2f} %"
            )


if __name__ == "__main__":
    main()
