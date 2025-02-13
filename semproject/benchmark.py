import time
import semproject.graph as graph_module
import semproject.strats as strats_module
import semproject.louvain as louvain_module
import argparse
import os
import subprocess
import shutil

def graph_benchmark():
    print("Running Graph tests...")

    try:
        subprocess.run(["unzip", "./data/input/twitch.zip", "-d", "./data/input/twitch/"], stdout=subprocess.DEVNULL, check=True)
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while unzipping the file: {e}")

    edges_path = "./data/input/twitch/twitch_edges.txt"
    features_path = "./data/input/twitch/twitch_features.txt"

    start_time = time.time()
    graph_module.GraphDouble(edges_path, features_path)
    end_time = time.time()

    try:
        shutil.rmtree("./data/input/twitch")
    except Exception as e:
        print(f"An error occurred while deleting the folder: {e}")

    print(f"Graph operations completed in {end_time - start_time:.1f} seconds")

def strat_benchmark(strategy, data_set, knn_depth=3):
    strategy = strategy.lower()
    data_set_case = data_set # for printing
    data_set = data_set.lower()
    if strategy == "knn":
        print(f"Running kNN imputation on '{data_set_case}' with depth {knn_depth}...")
    elif strategy == "louvain":
        print(f"Running Louvain imputation on '{data_set_case}' ...")
    elif strategy == "deepwalk":
        print("DeepWalk is not implemented, yet.")
        # print(f"Running DeepWalk imputation on '{data_set_case}' ...")


    graph_class_map = {
        "amazon": graph_module.GraphBool,
        "genius": graph_module.GraphInt,
        "twitch": graph_module.GraphDouble,
        "github": graph_module.GraphDouble,
        "corafull": graph_module.GraphBool,
        "amazon_fraud": graph_module.GraphDouble
    }
    graph_class = graph_class_map.get(data_set)
    if not graph_class:
        print(f"No graph class found for {data_set_case}, exiting...")
        return

    input_zip_path = f"./data/input/{data_set}.zip"
    input_path = f"./data/input/{data_set}/"
    edges_path = f"./data/input/{data_set}/{data_set}_edges.txt"
    features_path = f"./data/input/{data_set}/{data_set}_features.txt"
    output_path = f"./data/output/{strategy}/{data_set}_features.txt"

    try:
        subprocess.run(["unzip", input_zip_path, "-d", input_path], stdout=subprocess.DEVNULL, check=True)
    except subprocess.CalledProcessError:
        raise Exception("Failed to unzip the dataset.")

    graph = graph_class(edges_path, features_path)

    start_time = time.time()
    if strategy == "knn":
        knn_imputer = strats_module.KNNImputer(graph)
        knn_imputer.set_depth(knn_depth)
        knn_imputer.run()
    elif strategy == "louvain":
        louvain = louvain_module.Louvain(graph)

        start_time2 = time.time()
        communities = louvain.execute()
        end_time2 = time.time()
        print(f"Louvain community detection on '{data_set_case}' completed in {end_time2 - start_time2:.1f} seconds")

        louvain_imputer = strats_module.CommunityImputer(graph, communities)
        louvain_imputer.run()
    elif strategy == "deepwalk":
        return
    end_time = time.time()
    
    print(f"{strategy} benchmark on '{data_set_case}' benchmark completed in {end_time - start_time:.1f} seconds")

    graph.print_features_to_file(output_path)

    try:
        shutil.rmtree(input_path)
    except Exception as e:
        print(f"An error occurred while deleting the folder: {e}")

def main():
    strats = ["knn", "louvain", "deepwalk"]
    data_sets = ["twitch", "amazon_fraud", "corafull", "genius", "amazon", "github"]

    parser = argparse.ArgumentParser(description="Benchmarking different strategies.")
    parser.add_argument("--graph", action="store_true", help="Only benchmark graph loading.")
    parser.add_argument("--strat", type=str, choices=strats, help="The strategy to benchmark.")
    parser.add_argument("--input", type=str, choices=data_sets, help="The data set to use.")
    
    args = parser.parse_args()
    if args.graph:
        graph_benchmark()
    elif args.strat == "knn":
        if args.input:
            strat_benchmark(args.strat, args.input, knn_depth=3)
    elif args.strat == "louvain":
        if args.input:
            strat_benchmark(args.strat, args.input)
    elif args.strat == "deepwalk":
        print("Not implemented yet. Exiting...")
        return
        if args.input:
            strat_benchmark(args.strat, args.input)
    else:
        print("Invalid arguments. Exiting...")
        return
    
if __name__ == "__main__":
    main()