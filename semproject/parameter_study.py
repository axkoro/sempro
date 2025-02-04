from semproject import benchmark as bm

import sys
import subprocess
import re
import numpy as np
import matplotlib.pyplot as plt

def plot_strategy_comparison():
    data_sets = ["twitch", "amazon", "genius", "github"] # "corafull" excluded due to current memory issues with evaluation script
    strats = ["knn", "louvain"]

    metrics = {
        "overlap_total": {},
        "overlap_missing": {},
        "error_max": {},
        "error_avg": {},
        "r2": {}
    }

    for data_set in data_sets:
        for strat in strats:
            bm.strat_benchmark(strategy=strat, data_set=data_set)
            evaluation_results = run_evaluation(strat=strat, data_set=data_set)
            for metric, metric_dict in metrics.items():
                # setdefault ensures there's a dict for this data_set
                metric_dict.setdefault(data_set, {})[strat] = evaluation_results[metric]

    # Prepare 2x2 subplots for the four metrics (excluding overlap_total)
    chart_metrics = ["overlap_missing", "error_max", "error_avg", "r2"]
    fig, axs = plt.subplots(nrows=2, ncols=2, figsize=(12, 10))
    axs = axs.flatten()

    bar_width = 0.35
    x = np.arange(len(data_sets))

    for idx, metric in enumerate(chart_metrics):
        ax = axs[idx]
        for i, strat in enumerate(strats):
            # Get the metric values for each data_set for the current strategy
            values = [metrics[metric][ds][strat] for ds in data_sets]
            # Position bars side by side
            offset = (i - 0.5) * bar_width
            ax.bar(x + offset, values, bar_width, label=strat)
        ax.set_title(metric)
        ax.set_xticks(x)
        ax.set_xticklabels(data_sets)
        ax.legend()

    fig.suptitle("kNN vs. Louvain", fontsize=16)
    plt.subplots_adjust(hspace=0.5, wspace=0.3)
    plt.savefig("strategies.png".lower(), dpi=300)
    plt.close(fig)

def plot_knn_depth():
    depths = [1, 2, 3, 4]
    data_sets = ["twitch", "amazon", "genius", "github"] # "corafull" excluded due to current memory issues with evaluation script

    results = {}
    for data_set in data_sets:
        overlap_total = []
        overlap_missing = []
        error_max = []
        error_avg = []
        r2 = []

        for depth in depths:
            bm.strat_benchmark(strategy="knn", data_set=data_set, knn_depth=depth)
            evaluation_results = run_evaluation(strat="knn", data_set=data_set)

            overlap_total.append(evaluation_results["overlap_total"])
            overlap_missing.append(evaluation_results["overlap_missing"])
            error_max.append(evaluation_results["error_max"])
            error_avg.append(evaluation_results["error_avg"])
            r2.append(evaluation_results["r2"])
        
        results[data_set] = {"overlap_total": overlap_total,
                                "overlap_missing": overlap_missing,
                                "error_max": error_max,
                                "error_avg": error_avg,
                                "r2": r2}

    # Plot
    fig, axs = plt.subplots(nrows=2, ncols=2)
    fig.suptitle(f"kNN - Depth", fontsize=16)
    plt.subplots_adjust(hspace=0.5, wspace=0.5)

    data_set_colors = {"twitch": "purple", "amazon" : "orange", "amazon_fraud": "red", "corafull" : "blue", "genius" : "yellow", "github" : "green"}

    axs = axs.flatten()
    for ax in axs:
        ax.set_xlabel(f"Depth")
        ax.set_xticks(depths)

    for data_set in data_sets:
        axs[0].plot(depths, results[data_set]["overlap_missing"], marker="o", color=data_set_colors[data_set], label=data_set)
    axs[0].set_ylabel("Overlap Missing (%)")
    ax[0].legend()

    # TODO: normalise this somehow (genius max error is 1e6 while amazon max error is 1 which becomes invisible on the diagram)
    for data_set in data_sets:
        axs[1].plot(depths, results[data_set]["error_max"], marker="o", color=data_set_colors[data_set], label=data_set)
    axs[1].set_ylabel("Maximum Error (abs)")
    ax[1].legend()

    for data_set in data_sets:
        axs[2].plot(depths, results[data_set]["error_avg"], marker="o", color=data_set_colors[data_set], label=data_set)
    axs[2].set_ylabel("Average Error (abs)")
    ax[2].legend()

    for data_set in data_sets:
        axs[3].plot(depths, results[data_set]["r2"], marker="o", color=data_set_colors[data_set], label=data_set)
    axs[3].set_ylabel("R2 Score")
    ax[3].legend()

    plt.savefig(f"knn_depth.png".lower(), dpi=300)
    plt.close(fig)

def run_evaluation(strat, data_set):
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
    Parse the evaluation output and return the results.

    Expected output example:
        twitch overlap (total): 60.08%
        twitch overlap (missing): 0.00%
        twitch max error (abs): 10.06
        twitch avg error (abs): 0.20
        twitch R2 score: 0.60

    Returns:
        A dict {"overlap_total", "overlap_missing", "error_max", "error_avg", "r2"}
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