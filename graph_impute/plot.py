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


def main() -> None:
    figures = plot_parameter(
        strategy="community",
        parameter_name="max_levels",
        parameter_values=[1, 3, 10],
        fixed_parameters={},
        datasets=["amazon", "twitch"],
        metrics=["overlap_missing", "avg_error", "r2_score"],
    )
    save_plots_to_pdf(figures, "community_max_levels.pdf")


if __name__ == "__main__":
    main()
