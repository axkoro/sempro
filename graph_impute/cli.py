import argparse
import time
from textwrap import dedent

import yaml

from .evaluation import evaluate_imputed_features_file
from .graph import Graph
from .imputation import create_imputer


def parse_args():
    parser = argparse.ArgumentParser(description="Graph imputation CLI tool.")

    parser.add_argument(
        "--strategy",
        choices=["knn", "community", "deepwalk"],
        required=True,
        help="Imputation strategy to use.",
    )

    parser.add_argument(
        "--dataset", type=str, help='Name of a pre-configured dataset (e.g., "amazon").'
    )

    parser.add_argument(
        "--edges",
        type=str,
        help='Path to the file containing edge data. Defaults to searching for "*edges.txt" in the working directory.',
    )

    parser.add_argument(
        "--features",
        type=str,
        help='Path to the file containing feature data. Defaults to searching for "*features.txt" in the working directory.',
    )

    parser.add_argument(
        "--output",
        type=str,
        default="./imputed_features.txt",
        help="Path to store the imputed feature data. Default is './imputed_features.txt'.",
    )

    parser.add_argument(
        "--feature-type",
        choices=["bool", "int", "float"],
        default="float",
        help="Data type of the features in the graph. Default is 'float'.",
    )

    parser.add_argument(
        "--evaluate", action="store_true", help="Evaluate the quality of the imputed features."
    )

    parser.add_argument(
        "--reference",
        type=str,
        help="Path to the reference data for evaluation when --evaluate is passed.",
    )

    parser.add_argument(
        "--time", action="store_true", help="Time all major substeps of the process."
    )

    parser.add_argument(
        "--config",
        type=str,
        default="./datasets.yaml",
        help="Path for the configuration file mapping dataset names to file paths. Default is './datasets.yaml'.",
    )

    return parser.parse_args()


def validate_command(args):
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


def main():
    args = parse_args()
    validate_command(args)

    # get file paths & feature_type
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
        except IndexError as err:
            raise LookupError(
                f"Could not find all relevant data for data set '{args.dataset}' in '{config_path}' (see documentation for information about the config)"
            ) from err
    else:
        feature_type_str = args.feature_type
        edges_path = args.edges
        features_path = args.features
        if args.evaluate:
            reference_path = args.reference

    type_map = {
        "bool": bool,
        "int": int,
        "float": float,
    }
    feature_type = type_map[feature_type_str]

    # run strategy (possibly with timing)
    load_time_start = time.time()
    graph = Graph.load(edges_path, features_path, feature_type=feature_type)
    imputer = create_imputer(args.strategy, graph)
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
        total_time = save_time_end - load_time_start
        load_time = load_time_end - load_time_start
        impute_time = impute_time_end - impute_time_start
        save_time = save_time_end - save_time_start

        times = [total_time, load_time, impute_time, save_time]
        formatted_times = [f"{t:.2f}" for t in times]
        max_width = max(len(t_str) for t_str in formatted_times)

        output = dedent(f"""\
            Total time:  {total_time:{max_width}.2f} s
            Impute time: {impute_time:{max_width}.2f} s
            Load time:   {load_time:{max_width}.2f} s
            Save time:   {save_time:{max_width}.2f} s""")

        if args.strategy == "community":
            community_detection_time = community_detection_time_end - community_detection_time_start
            output.append(f"Community detection time: {community_detection_time:{max_width}.2f} s")

        print(output)

    if args.evaluate:
        overlap_total, overlap_missing, max_error, avg_error, r2_score = (
            evaluate_imputed_features_file(
                imputed_features_path=args.output,
                reference_path=reference_path,
                input_features_path=features_path,
                feature_type=feature_type,
            )
        )

        print(f"overlap (total): {overlap_total * 100:2.2f} %")
        print(f"overlap (missing): {overlap_missing * 100:2.2f} %")

        print(f"max error (abs): {max_error:2.2f}")
        print(f"avg error (abs): {avg_error:2.2f}")

        print(f"R2 score: {r2_score:2.2f}")


if __name__ == "__main__":
    main()
