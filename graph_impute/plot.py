import os
import sqlite3
import time
from copy import deepcopy
from datetime import datetime
from typing import Any, Dict, List, NamedTuple, Tuple

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

from .cli import load_dataset_config
from .evaluation import evaluate_imputed_graph
from .graph import Graph
from .imputation import create_imputer


class _Result(NamedTuple):
    overlap_total: float = None
    overlap_missing: float = None
    max_error: float = None
    avg_error: float = None
    r2_score: float = None
    model_accuracy_original: float = None
    model_accuracy_imputed: float = None
    execution_time: float = None


class _Database:
    def __init__(self, db_file: str = "results.db") -> None:
        self.db_file = db_file
        self.conn = None
        self.cursor = None

    def __enter__(self) -> sqlite3.Cursor:
        db_existed = os.path.exists(self.db_file)
        self.conn = sqlite3.connect(self.db_file)
        self.cursor = self.conn.cursor()
        if not db_existed:
            self._init_db()
        return self.cursor

    def _init_db(self) -> None:
        self.cursor.execute("""
            CREATE TABLE IF NOT EXISTS results (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                dataset TEXT,
                strategy TEXT,
                timestamp TEXT,
                k INTEGER,
                use_k_hop BOOLEAN,
                community_algorithm TEXT,
                max_levels INTEGER,
                max_iterations INTEGER,
                tolerance REAL,
                no_edge_weights BOOLEAN,
                fusion_coefficient REAL,
                walk_length INTEGER,
                num_walks INTEGER,
                embedding_size INTEGER,
                context_window INTEGER,
                num_negative_samples INTEGER,
                smoothing_exponent REAL,
                num_epochs INTEGER,
                learning_rate REAL,
                top_similar INTEGER,
                similarity_metric TEXT,
                overlap_total REAL,
                overlap_missing REAL,
                max_error REAL,
                avg_error REAL,
                r2_score REAL,
                model_accuracy_original REAL,
                model_accuracy_imputed REAL,
                execution_time REAL
            )
        """)
        self.conn.commit()

    def __exit__(self, exc_type, exc_value, traceback) -> None:
        self.conn.commit()
        self.conn.close()


def _execute_strategy(
    strategy: str, dataset: str, parameters: Dict[str, Any]
) -> Tuple[_Result, Dict]:
    """
    Executes the given strategy on the dataset with the provided parameters.
    Returns a tuple of (_Result, config) where config is obtained from imputer.get_config().
    """
    edges_path, features_path, reference_path, feature_type = load_dataset_config(
        config_path="datasets.yaml",
        dataset=dataset,
        evaluate=True,
    )

    graph = Graph.load(
        edges_file=edges_path, features_file=features_path, feature_type=feature_type
    )
    imputer = create_imputer(strategy, graph, **parameters)
    config = imputer.get_config()

    start_time = time.perf_counter()
    imputer.impute()
    execution_time = time.perf_counter() - start_time

    results = evaluate_imputed_graph(graph, features_path, reference_path)

    result_obj = _Result(
        overlap_total=results["overlap_total"],
        overlap_missing=results["overlap_missing"],
        max_error=results["max_error"],
        avg_error=results["avg_error"],
        r2_score=results["r2_score"],
        model_accuracy_original=None,
        model_accuracy_imputed=None,
        execution_time=execution_time,
    )
    return result_obj, config


def _get_results(
    strategy: str,
    parameter_name: str,
    parameter_values: List[Any],
    fixed_parameters: Dict[str, Any],
    datasets: List[str],
) -> Dict[str, List[_Result]]:
    """
    Retrieves or calculates results for each dataset and each value of the variable parameter.
    """
    all_results = {}  # dataset -> list of _Result

    for dataset in datasets:
        edges_path, features_path, reference_path, feature_type = load_dataset_config(
            config_path="datasets.yaml",
            dataset=dataset,
            evaluate=True,
        )
        graph = Graph.load(
            edges_file=edges_path, features_file=features_path, feature_type=feature_type
        )
        default_config = create_imputer(strategy, graph).get_config()
        config_keys = sorted(default_config.keys())

        dataset_results = []
        for value in parameter_values:
            effective_params = deepcopy(default_config)
            effective_params.update(fixed_parameters)
            effective_params[parameter_name] = value

            fixed_keys = sorted(fixed_parameters.keys())
            conditions = ["dataset = ?", "strategy = ?", f"{parameter_name} = ?"] + [
                f"{k} = ?" for k in fixed_keys
            ]
            values_list = [dataset, strategy, effective_params[parameter_name]] + [
                effective_params[k] for k in fixed_keys
            ]

            query = (
                "SELECT overlap_total, overlap_missing, max_error, avg_error, r2_score, "
                "model_accuracy_original, model_accuracy_imputed, execution_time FROM results WHERE "
                + " AND ".join(conditions)
                + " ORDER BY timestamp DESC LIMIT 1"
            )

            with _Database() as cursor:
                cursor.execute(query, tuple(values_list))
                row = cursor.fetchone()

            if row is None:
                result_obj, config = _execute_strategy(strategy, dataset, effective_params)
                if not config_keys:
                    config_keys = sorted(config.keys())

                timestamp = datetime.now().isoformat()
                columns = [
                    "dataset",
                    "strategy",
                    "timestamp",
                    *config_keys,
                    "overlap_total",
                    "overlap_missing",
                    "max_error",
                    "avg_error",
                    "r2_score",
                    "model_accuracy_original",
                    "model_accuracy_imputed",
                    "execution_time",
                ]
                placeholders = ", ".join("?" * len(columns))
                insert_query = f"INSERT INTO results ({', '.join(columns)}) VALUES ({placeholders})"

                # Convert numeric fields to native float
                row_values = (
                    dataset,
                    strategy,
                    timestamp,
                    *(config.get(key) for key in config_keys),
                    float(result_obj.overlap_total)
                    if result_obj.overlap_total is not None
                    else None,
                    float(result_obj.overlap_missing)
                    if result_obj.overlap_missing is not None
                    else None,
                    float(result_obj.max_error) if result_obj.max_error is not None else None,
                    float(result_obj.avg_error) if result_obj.avg_error is not None else None,
                    float(result_obj.r2_score) if result_obj.r2_score is not None else None,
                    float(result_obj.model_accuracy_original)
                    if result_obj.model_accuracy_original is not None
                    else None,
                    float(result_obj.model_accuracy_imputed)
                    if result_obj.model_accuracy_imputed is not None
                    else None,
                    float(result_obj.execution_time)
                    if result_obj.execution_time is not None
                    else None,
                )
                with _Database() as cursor:
                    cursor.execute(insert_query, row_values)
                print(f"Calculated result for {parameter_name}={value} on {dataset}.")
                dataset_results.append(result_obj)
            else:
                dataset_results.append(_Result(*row))
                print(f"Retrieved result for {parameter_name}={value} on {dataset} from database.")
        all_results[dataset] = dataset_results

    return all_results


def _create_plots(
    parameter_name: str,
    parameter_values: List[Any],
    metrics: List[str],
    datasets: List[str],
    results: Dict[str, List[_Result]],
) -> List[plt.Figure]:
    """
    Creates figures (one per dataset) with a subplot for each metric.
    """
    figures = []
    for dataset in datasets:
        num_metrics = len(metrics)
        fig, axes = plt.subplots(nrows=1, ncols=num_metrics, figsize=(6 * num_metrics, 5))
        if num_metrics == 1:
            axes = [axes]

        for ax, metric in zip(axes, metrics):
            metric_vals = [getattr(result, metric) for result in results[dataset]]
            ax.plot(parameter_values, metric_vals, marker="o")
            ax.set_title(f"{dataset} - {metric}")
            ax.set_xlabel(parameter_name)
            ax.set_ylabel(metric)
            ax.grid(True)

        fig.tight_layout()
        figures.append(fig)
    return figures


def save_plots_to_pdf(figures: List[plt.Figure], filename: str) -> None:
    """
    Saves figures to a PDF, one per page.
    """
    with PdfPages(filename) as pdf:
        for fig in figures:
            pdf.savefig(fig)
            plt.close(fig)
    print(f"Plots saved to {filename}")


def plot_parameter(
    strategy: str,
    parameter_name: str,
    parameter_values: List[Any],
    fixed_parameters: Dict[str, Any],
    datasets: List[str],
    metrics: List[str],
) -> List[plt.Figure]:
    first_dataset = datasets[0]
    edges_path, features_path, reference_path, feature_type = load_dataset_config(
        config_path="datasets.yaml",
        dataset=first_dataset,
        evaluate=True,
    )
    graph = Graph.load(
        edges_file=edges_path, features_file=features_path, feature_type=feature_type
    )
    default_config = create_imputer(strategy, graph).get_config()

    if parameter_name not in default_config:
        raise ValueError(
            f"Variable parameter '{parameter_name}' is not valid for strategy '{strategy}'."
        )

    for key in fixed_parameters.keys():
        if key not in default_config:
            raise ValueError(f"Fixed parameter '{key}' is not valid for strategy '{strategy}'.")

    results = _get_results(strategy, parameter_name, parameter_values, fixed_parameters, datasets)
    return _create_plots(parameter_name, parameter_values, metrics, datasets, results)


def plot_best_results(
    strategies: List[str],
    datasets: List[str],
    metrics: List[str],
    db_file: str = "results.db",
) -> List[plt.Figure]:
    """
    Creates bar plots for the best results (per metric) stored in the database.
    For each metric (e.g. overlap_missing, avg_error, r2_score):
      - x-ticks represent datasets.
      - At each dataset tick, one bar per strategy (knn, community, deepwalk) is plotted,
        corresponding to the best result found in the database.
      - Each bar is annotated with its execution time.

    Note: For overlap_missing and avg_error lower values are better (ascending order),
    whereas for r2_score higher values are preferred (descending order).
    """
    import numpy as np

    # Define ordering direction for each metric: ASC means lower is better, DESC means higher is better.
    metric_order = {
        "overlap_missing": "ASC",
        "avg_error": "ASC",
        "r2_score": "DESC",
    }

    # Dictionary to hold best results:
    # Structure: best_results[metric][dataset][strategy] = (metric_value, execution_time)
    best_results: Dict[str, Dict[str, Dict[str, Tuple[Optional[float], Optional[float]]]]] = {
        metric: {} for metric in metrics
    }

    # Loop over metrics, datasets, and strategies to query the best result.
    for metric in metrics:
        for dataset in datasets:
            if dataset not in best_results[metric]:
                best_results[metric][dataset] = {}
            for strategy in strategies:
                query = (
                    f"SELECT {metric}, execution_time FROM results "
                    f"WHERE dataset=? AND strategy=? "
                    f"ORDER BY {metric} {metric_order[metric]} LIMIT 1"
                )
                with _Database(db_file) as cursor:
                    cursor.execute(query, (dataset, strategy))
                    row = cursor.fetchone()
                if row is None:
                    best_results[metric][dataset][strategy] = (None, None)
                else:
                    best_results[metric][dataset][strategy] = (
                        row  # row: (metric_value, execution_time)
                    )

    # Create bar plots for each metric.
    figures: List[plt.Figure] = []
    for metric in metrics:
        fig, ax = plt.subplots(figsize=(10, 6))
        x = np.arange(len(datasets))
        n_strat = len(strategies)
        bar_width = 0.8 / n_strat  # distribute bars within each dataset tick
        # Compute offsets to center the grouped bars at each dataset tick.
        offsets = np.linspace(-0.4 + bar_width / 2, 0.4 - bar_width / 2, n_strat)

        for i, strategy in enumerate(strategies):
            values = []
            exec_times = []
            for dataset in datasets:
                value, exec_time = best_results[metric][dataset].get(strategy, (None, None))
                # If no result is found, plot zero and annotate as "N/A"
                if value is None:
                    values.append(0)
                    exec_times.append(None)
                else:
                    values.append(value)
                    exec_times.append(exec_time)
            positions = x + offsets[i]
            bars = ax.bar(positions, values, width=bar_width, label=strategy)
            # Annotate each bar with its execution time.
            for bar, t in zip(bars, exec_times):
                height = bar.get_height()
                label = "N/A" if t is None else f"{t:.2f}s"
                ax.text(
                    bar.get_x() + bar.get_width() / 2,
                    height,
                    label,
                    ha="center",
                    va="bottom",
                    fontsize=8,
                )

        ax.set_xticks(x)
        ax.set_xticklabels(datasets)
        ax.set_xlabel("Dataset")
        ax.set_ylabel(metric)
        ax.set_title(f"Best {metric} across strategies")
        ax.legend()
        fig.tight_layout()
        figures.append(fig)
    return figures


def main() -> None:
    param_name = "fusion_coefficient"
    param_values = [0.01, 0.2, 0.4, 0.5, 0.6, 0.8, 0.99]
    figures = []
    figures.extend(
        plot_parameter(
            strategy="deepwalk",
            parameter_name=param_name,
            parameter_values=param_values,
            fixed_parameters={
                "no_edge_weights": False,
                "walk_length": 80,
                "num_walks": 15,
                "embedding_size": 16,
                "context_window": 10,
                "num_negative_samples": 20,
                "similarity_metric": "cosine",
                "top_similar": 25,
            },
            datasets=["twitch"],
            metrics=["overlap_missing", "avg_error", "r2_score", "execution_time"],
        )
    )
    figures.extend(
        plot_parameter(
            strategy="deepwalk",
            parameter_name=param_name,
            parameter_values=param_values,
            fixed_parameters={
                "no_edge_weights": True,
                "walk_length": 21,
                "num_walks": 10,
                "embedding_size": 64,
                "context_window": 2,
                "num_negative_samples": 15,
                "similarity_metric": "dot_product",
                "top_similar": 10,
            },
            datasets=["cora"],
            metrics=["overlap_missing", "avg_error", "r2_score", "execution_time"],
        )
    )
    figures.extend(
        plot_parameter(
            strategy="deepwalk",
            parameter_name=param_name,
            parameter_values=param_values,
            fixed_parameters={
                "no_edge_weights": False,
                "walk_length": 80,
                "num_walks": 20,
                "embedding_size": 16,
                "context_window": 15,
                "num_negative_samples": 15,
                "similarity_metric": "cosine",
                "top_similar": 100,
            },
            datasets=["amazon"],
            metrics=["overlap_missing", "avg_error", "r2_score", "execution_time"],
        )
    )
    figures.extend(
        plot_parameter(
            strategy="deepwalk",
            parameter_name=param_name,
            parameter_values=param_values,
            fixed_parameters={
                "no_edge_weights": False,
                "fusion_coefficient": 0.6,
                "walk_length": 40,
                "num_walks": 10,
                "embedding_size": 64,
                "context_window": 10,
                "num_negative_samples": 10,
                "similarity_metric": "cosine",
                "top_similar": 100,
            },
            datasets=["github"],
            metrics=["overlap_missing", "avg_error", "r2_score", "execution_time"],
        )
    )
    # figures.extend(
    #     plot_parameter(
    #         strategy="deepwalk",
    #         parameter_name=param_name,
    #         parameter_values=[0, 0.5, 0.6, 0.99],
    #         fixed_parameters={
    #             "no_edge_weights": True,
    #             "embedding_size": 64,
    #             "walk_length": 40,
    #             "num_walks": 5,
    #             "context_window": 10,
    #             "num_negative_samples": 5,
    #             "similarity_metric": "dot_product",
    #             "top_similar": 25,
    #         },
    #         datasets=["genius"],
    #         metrics=["overlap_missing", "avg_error", "r2_score", "execution_time"],
    #     )
    # )
    save_plots_to_pdf(figures, "deepwalk_fusion_coefficient.pdf")


if __name__ == "__main__":
    main()
