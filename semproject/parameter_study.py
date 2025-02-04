from semproject import benchmark as bm

import sys
import subprocess
import re
import matplotlib.pyplot as plt

def main():
    # knn depth on twitch
    depths = [1, 2, 3, 4]
    overlap_total = []
    overlap_missing = []
    error_max = []
    error_avg = []
    r2 = []

    for depth in depths:
        bm.strat_benchmark("knn", "twitch", knn_depth=depth)
        results = evaluate_strat("knn", "twitch")
        overlap_total.append(results["overlap_total"])
        overlap_missing.append(results["overlap_missing"])
        error_max.append(results["error_max"])
        error_avg.append(results["error_avg"])
        r2.append(results["r2"])

    plot_quality_measures(strategy_name="kNN",
                          data_set_name="Twitch",
                          parameter_name="depth",
                          parameter_list=depths,
                          overlap_total=overlap_total,
                          overlap_missing=overlap_missing,
                          error_max=error_max,
                          error_avg=error_avg,
                          r2=r2)
    
    # louvain
    depths = [1, 2, 3, 4]
    overlap_total = []
    overlap_missing = []
    error_max = []
    error_avg = []
    r2 = []

    for depth in depths:
        bm.strat_benchmark("louvain", "twitch", knn_depth=depth)
        results = evaluate_strat("knn", "twitch")
        overlap_total.append(results["overlap_total"])
        overlap_missing.append(results["overlap_missing"])
        error_max.append(results["error_max"])
        error_avg.append(results["error_avg"])
        r2.append(results["r2"])

    plot_quality_measures(strategy_name="kNN",
                          data_set_name="Twitch",
                          parameter_name="depth",
                          parameter_list=depths,
                          overlap_total=overlap_total,
                          overlap_missing=overlap_missing,
                          error_max=error_max,
                          error_avg=error_avg,
                          r2=r2)

def plot_quality_measures(strategy_name, data_set_name, parameter_name, parameter_list, overlap_total, overlap_missing, error_max, error_avg, r2):
    """
    Plot quality measures versus kNN depth. Each measure is plotted in its own graph.

    Args:
        strategy_name (string): The name of strategy that was evaluated.
        parameter_name (string): The name of the data set on which that strategy was run.
        parameter_name (string): The name of the parameter to be plotted.
        parameter_list (list): List of the different parameter values that were run and evaluated.
        overlap_total (list): Overlap total percentages.
        overlap_missing (list): Overlap missing percentages.
        error_max (list): Maximum error values.
        error_avg (list): Average error values.
        r2 (list): R2 score values.
    """
    fig, axs = plt.subplots(nrows=2, ncols=2)
    fig.suptitle(f"{strategy_name} - {parameter_name} ({data_set_name})")
    plt.subplots_adjust(hspace=0.5)
    plt.subplots_adjust(wspace=0.5)

    axs = axs.flatten()
    for ax in axs:
        ax.set_xlabel(f"{parameter_name}")
        axs[0].set_xticks(parameter_list)
        

    axs[0].plot(parameter_list, overlap_missing, marker="o", color="orange")
    axs[0].set_ylabel("Overlap Missing (%)")

    axs[1].plot(parameter_list, error_max, marker="o", color="green")
    axs[1].set_ylabel("Maximum Error (abs)")

    axs[2].plot(parameter_list, error_avg, marker="o", color="red")
    axs[2].set_ylabel("Average Error (abs)")

    axs[3].plot(parameter_list, r2, marker="o", color="purple")
    axs[3].set_ylabel("R2 Score")

    plt.savefig(f"{strategy_name}_{parameter_name}_{data_set_name}.png".lower())
    plt.close(fig)

def evaluate_strat(strat, data_set):
    evaluation_cmd = [
        sys.executable,
        "./extlibs/evaluation/measure-quality.py",
        "--instance", data_set,
        "--input-folder", "./data/input",
        "--feature-folder", f"./data/output/{strat}",
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

if __name__ == "__main__":
    main()