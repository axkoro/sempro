from semproject import benchmark as bm
import sys
import subprocess
import re
import numpy as np
import matplotlib.pyplot as plt

DATA_SETS = ["twitch", "amazon", "genius", "github"]  # "corafull" excluded due to memory issues
STRATEGIES = ["knn", "louvain"]
METRICS = ["overlap_total", "overlap_missing", "error_max", "error_avg", "r2"]

DATA_SET_COLORS = {
    "twitch": "purple",
    "amazon": "orange",
    "amazon_fraud": "red",
    "corafull": "blue",
    "genius": "yellow",
    "github": "green"
}


def evaluate_strategy(strategy, data_set, **benchmark_kwargs):
    """
    Run the benchmark and evaluation for a given strategy and data set.
    """
    bm.strat_benchmark(strategy=strategy, data_set=data_set, **benchmark_kwargs)
    return run_evaluation(strat=strategy, data_set=data_set)


def plot_strategy_comparison():
    """
    Compare STRATEGIES over the DATA_SETS.
    """
    # Prepare a nested dict: metric -> data_set -> strategy -> value
    metrics = {metric: {} for metric in METRICS}
    for data_set in DATA_SETS:
        for strat in STRATEGIES:
            eval_results = evaluate_strategy(strategy=strat, data_set=data_set)
            for metric in METRICS:
                metrics[metric].setdefault(data_set, {})[strat] = eval_results[metric]

    # Chart only selected metrics (exclude overlap_total)
    chart_metrics = ["overlap_missing", "error_max", "error_avg", "r2"]
    fig, axs = plt.subplots(nrows=2, ncols=2, figsize=(12, 10))
    axs = axs.flatten()
    bar_width = 0.35
    x = np.arange(len(DATA_SETS))

    for idx, metric in enumerate(chart_metrics):
        ax = axs[idx]
        for i, strat in enumerate(STRATEGIES):
            # Get the metric value for each data set for the current strategy
            values = [metrics[metric][ds][strat] for ds in DATA_SETS]
            offset = (i - 0.5) * bar_width
            ax.bar(x + offset, values, bar_width, label=strat)
        ax.set_title(metric)
        ax.set_xticks(x)
        ax.set_xticklabels(DATA_SETS)
        ax.legend(loc="upper right")

    fig.suptitle("kNN vs. Louvain", fontsize=16)
    plt.subplots_adjust(hspace=0.5, wspace=0.3)
    plt.savefig("strategies.png", dpi=300)
    plt.close(fig)


def plot_knn_depth():
    """
    Plot evaluation metrics for the kNN strategy over various depth settings.
    """
    depths = [1, 2, 3, 4]
    results = {}  # data_set -> metric -> list of values

    # Evaluate kNN for each data set at different depths
    for data_set in DATA_SETS:
        data_results = {metric: [] for metric in METRICS}
        for depth in depths:
            eval_results = evaluate_strategy(strategy="knn", data_set=data_set, knn_depth=depth)
            for metric in METRICS:
                data_results[metric].append(eval_results[metric])
        results[data_set] = data_results

    # Define metrics (other than overlap_total) to plot along with their y-axis labels
    line_metrics = {
        "overlap_missing": "Overlap Missing (%)",
        "error_max": "Maximum Error (abs)",
        "error_avg": "Average Error (abs)",
        "r2": "R2 Score"
    }

    fig, axs = plt.subplots(nrows=2, ncols=2, figsize=(12, 10))
    axs = axs.flatten()
    fig.suptitle("kNN - Depth", fontsize=16)
    plt.subplots_adjust(hspace=0.5, wspace=0.5)

    # Set common x-axis properties
    for ax in axs:
        ax.set_xlabel("Depth")
        ax.set_xticks(depths)

    # Plot each metric
    for ax, (metric, ylabel) in zip(axs, line_metrics.items()):
        for data_set in DATA_SETS:
            ax.plot(
                depths,
                results[data_set][metric],
                marker="o",
                color=DATA_SET_COLORS.get(data_set, "black"),
                label=data_set
            )
        ax.set_ylabel(ylabel)
        ax.legend(loc="upper right")

    plt.savefig("knn_depth.png", dpi=300)
    plt.close(fig)


def run_evaluation(strat, data_set):
    """
    Run the evaluation script as a subprocess and parse its output.
    """
    evaluation_cmd = [
        sys.executable,
        "./extlibs/evaluation/measure-quality.py",
        "--instance", data_set.lower(),
        "--input-folder", "./data/input",
        "--feature-folder", f"./data/output/{strat.lower()}",
        "--reference-folder", "./data/reference",
        "--txt"
    ]
    process = subprocess.run(
        evaluation_cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        text=True
    )
    return parse_evaluation_output(process.stdout)


def parse_evaluation_output(output: str):
    """
    Parse evaluation output and return a dict of metric values.

    Expected output example:
        twitch overlap (total): 60.08%
        twitch overlap (missing): 0.00%
        twitch max error (abs): 10.06
        twitch avg error (abs): 0.20
        twitch R2 score: 0.60
    """
    patterns = {
        "overlap_total": r"overlap \(total\):\s*([\d.]+)%",
        "overlap_missing": r"overlap \(missing\):\s*([\d.]+)%",
        "error_max": r"max error \(abs\):\s*([\d.]+)",
        "error_avg": r"avg error \(abs\):\s*([\d.]+)",
        "r2": r"R2 score:\s*([\d.]+)"
    }
    results = {}
    for key, pattern in patterns.items():
        match = re.search(pattern, output)
        if match:
            results[key] = float(match.group(1))
        else:
            raise RuntimeError(f"Unable to parse {key} from evaluation output")
    return results


if __name__ == "__main__":
    # Example usage
    plot_strategy_comparison()
    plot_knn_depth()
