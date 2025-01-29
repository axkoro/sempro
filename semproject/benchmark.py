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
        subprocess.run(["unzip", "./data/input/twitch.zip", "-d", "./data/input/twitch/"], check=True)
        print("Unzipped twitch.zip successfully.")
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while unzipping the file: {e}")

    edges_path = "./data/input/twitch/twitch_edges.txt"
    features_path = "./data/input/twitch/twitch_features.txt"

    start_time = time.time()
    graph_module.GraphDouble(edges_path, features_path)
    end_time = time.time()

    #delete the unzipped folder
    os.remove("./data/input/twitch/twitch_edges.txt")
    os.remove("./data/input/twitch/twitch_features.txt")
    os.rmdir("./data/input/twitch/")

    print(f"Graph operations completed in {end_time - start_time:.4f} seconds")

def strat_benchmark(data_set, strategy, depth=3):
    print(f"Running {strategy} imputation on '{data_set}' with depth {depth}...")

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
        print(f"No graph class found for {data_set}, exiting...")
        return

    zip_path = f"./data/input/{data_set}.zip"
    folder_path = f"./data/input/{data_set}/"
    edges_path = f"./data/input/{data_set}/{data_set}_edges.txt"
    features_path = f"./data/input/{data_set}/{data_set}_features.txt"

    if strategy == "knn":
        output_path = f"./data/output/knn/{data_set}_features.txt"
    else:
        output_path = f"./data/output/louvain/{data_set}_features.txt"

    try:
        subprocess.run(["unzip", zip_path, "-d", folder_path], stdout=subprocess.DEVNULL, check=True)
    except subprocess.CalledProcessError:
        raise Exception("Failed to unzip the dataset.")

    graph = graph_class(edges_path, features_path)

    start_time = time.time()
    if strategy == "knn":
        knn_imputer = strats_module.KNNImputer(graph)
        knn_imputer.set_depth(depth)
        knn_imputer.run()
    elif strategy == "louvain":
        louvain = louvain_module.Louvain(graph)

        start_time2 = time.time()
        communities = louvain.execute()
        end_time2 = time.time()
        print(f"louvain community detection on '{data_set}' completed in {end_time2 - start_time2:.4f} seconds")

        louvain_imputer = strats_module.LouvainImputer(graph, communities)
        louvain_imputer.run()
    end_time = time.time()
    
    print(f"{strategy} benchmark on '{data_set}' benchmark completed in {end_time - start_time:.4f} seconds")

    graph.print_features_to_file(output_path)

    try:
        shutil.rmtree(folder_path)
    except Exception as e:
        print(f"An error occurred while deleting the folder: {e}")

def main():
    parser = argparse.ArgumentParser(description="Benchmarking different strategies.")
    parser.add_argument("--graph", action='store_true', help="Only benchmark graph loading.")
    parser.add_argument("--strat", type=str, choices=["knn", "louvain", "gnn"], help="The strategy to benchmark.")
    parser.add_argument("--input", type=str, choices=["twitch", "amazon_fraud", "corafull", "genius", "amazon", "github"], help="The data set to use.")
    
    args = parser.parse_args()
    if args.graph:
        graph_benchmark()
    elif args.strat == "knn":
        if args.input:
            strat_benchmark(args.input, "knn", depth=3)
    elif args.strat == "louvain":
        if args.input:
            strat_benchmark(args.input, "louvain")
    elif args.strat == "gnn":
        print("Not implemented yet. Exiting...")
        return
    else:
        print("Invalid arguments. Exiting...")
        return
    
if __name__ == "__main__":
    main()