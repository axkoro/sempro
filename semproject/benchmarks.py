import time
import semproject.graph as graph_module
import semproject.strats as strats_module
import argparse
import os
import subprocess
import shutil

def graph_benchmark():
    print("Running Graph tests...")
    start_time = time.time()
    # Create a Graph instance
    #unzip the twitch.zip file
    try:
        subprocess.run(["unzip", "../data/input/twitch.zip", "-d", "../data/input/twitch/"], check=True)
        print("Unzipped twitch.zip successfully.")
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while unzipping the file: {e}")
    edges_path = "../data/input/twitch/twitch_edges.txt"
    features_path = "../data/input/twitch/twitch_features.txt"
    graph = graph_module.GraphDouble(edges_path,features_path)

    end_time = time.time()

    #delete the unzipped folder
    os.remove("../data/input/twitch/twitch_edges.txt")
    os.remove("../data/input/twitch/twitch_features.txt")
    os.rmdir("../data/input/twitch/")

    print(f"Graph operations completed in {end_time - start_time:.4f} seconds")

def kNN_benchmark(data_set, depth=3):
    print(f"Running kNN {data_set} benchmark with depth {depth}...")

    graph_class_map = {
        "amazon": graph_module.GraphBool,
        "genius": graph_module.GraphInt,
        "twitch": graph_module.GraphDouble,
        "github": graph_module.GraphDouble,
        "cora_full": graph_module.GraphBool,
        "amazon_fraud": graph_module.GraphDouble
    }
    graph_class = graph_class_map.get(data_set)
    if not graph_class:
        print(f"No graph class found for {data_set}, exiting...")
        return

    zip_path = f"../data/input/{data_set}.zip"
    folder_path = f"../data/input/{data_set}/"
    edges_path = f"../data/input/{data_set}/{data_set}_edges.txt"
    features_path = f"../data/input/{data_set}/{data_set}_features.txt"
    output_path = f"../data/output/knn/{data_set}_features.txt"

    try:
        subprocess.run(["unzip", zip_path, "-d", folder_path], check=True)
        print(f"Unzipped {data_set}.zip successfully.")
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while unzipping the file: {e}")

    graph = graph_class(edges_path, features_path)
    knn_imputer = strats_module.KNNImputer(graph)
    knn_imputer.set_depth(depth)

    try:
        shutil.rmtree(folder_path)
        print(f"Deleted the unzipped {data_set} folder successfully.")
    except Exception as e:
        print(f"An error occurred while deleting the folder: {e}")

    start_time = time.time()
    knn_imputer.run()
    end_time = time.time()
    print(f"kNN {data_set} benchmark completed in {end_time - start_time:.4f} seconds")

    graph.print_features_to_file(output_path)

def main():
    parser = argparse.ArgumentParser(description="Benchmarking different strategies.")
    
    parser.add_argument("--graph", action='store_true', help="Only benchmark graph loading.")
    parser.add_argument("--strat", type=str, choices=["knn", "louvain", "gnn"], help="The strategy to benchmark.")
    parser.add_argument("--input", type=str, choices=["twitch", "amazon_fraud", "cora_full", "genius", "amazon", "github"], help="The data set to use.")
    
    #if --strat arg = knn and --input arg = twitch run kNN_twitch_benchmark
    args = parser.parse_args()
    if args.graph:
        graph_benchmark()
    elif args.strat == "knn":
        if args.input:
            kNN_benchmark(args.input)
    elif args.strat == "louvain":
        print("Not implemented yet. Exiting...")
        return
    elif args.strat == "gnn":
        print("Not implemented yet. Exiting...")
        return
    else:
        print("Invalid arguments. Exiting...")
        return
    
if __name__ == "__main__":
    main()