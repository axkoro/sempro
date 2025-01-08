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
    graph = graph_module.Graph(edges_path,features_path)

    end_time = time.time()

    #delete the unzipped folder
    os.remove("../data/input/twitch/twitch_edges.txt")
    os.remove("../data/input/twitch/twitch_features.txt")
    os.rmdir("../data/input/twitch/")

    print(f"Graph operations completed in {end_time - start_time:.4f} seconds")

def kNN_twitch_benchmark():
    print("Running kNN twitch benchmark with depth 3...")
    
    # Create a Graph instance
    #unzip the twitch.zip file
    try:
        subprocess.run(["unzip", "../data/input/twitch.zip", "-d", "../data/input/twitch/"], check=True)
        print("Unzipped twitch.zip successfully.")
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while unzipping the file: {e}")

    edges_path = "../data/input/twitch/twitch_edges.txt"
    features_path = "../data/input/twitch/twitch_features.txt"
    graph = graph_module.Graph(edges_path,features_path)
    
    # Create a KNNImputer instance
    knn_imputer = strats_module.KNNImputer(graph)
    knn_imputer.set_depth(3)
    
    # Run the KNNImputer
    start_time = time.time()
    knn_imputer.run()
    end_time = time.time()

    #delete the unzipped folder with nested folders
    try:
        shutil.rmtree("../data/input/twitch/")
        print("Deleted the unzipped folder successfully.")
    except Exception as e:
        print(f"An error occurred while deleting the folder: {e}")

    print(f"kNN twitch benchmark completed in {end_time - start_time:.4f} seconds")

def kNN_fraud_benchmark():
    print("Running kNN amazon_fraud benchmark with depth 3...")
    
    # Create a Graph instance
    #unzip the twitch.zip file
    try:
        subprocess.run(["unzip", "../data/input/amazon_fraud.zip", "-d", "../data/input/amazon_fraud/"], check=True)
        print("Unzipped amazon_fraud.zip successfully.")
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while unzipping the file: {e}")
    edges_path = "../data/input/amazon_fraud/amazon_fraud_edges.txt"
    features_path = "../data/input/amazon_fraud/amazon_fraud_features.txt"
    graph = graph_module.Graph(edges_path,features_path)
    
    # Create a KNNImputer instance
    knn_imputer = strats_module.KNNImputer(graph)
    knn_imputer.set_depth(3)
    
    # Run the KNNImputer
    start_time = time.time()
    knn_imputer.run()
    end_time = time.time()
    
    #delete the unzipped folder with nested folders
    try:
        shutil.rmtree("../data/input/amazon_fraud/")
        print("Deleted the unzipped folder successfully.")
    except Exception as e:
        print(f"An error occurred while deleting the folder: {e}")

    print(f"kNN amazon_fraud benchmark completed in {end_time - start_time:.4f} seconds")
    
def main():
    parser = argparse.ArgumentParser(description="Benchmarking different strategies.")
    
    parser.add_argument("--strat", type=str, choices=["knn", "louvain", "gnn", "Graph"], required=True, help="The strategy to benchmark.")
    parser.add_argument("--input", type=str, choices=["twitch","amazon_fraud"], required=False, help="The input file to use.")

    #if --strat arg = knn and --input arg = twitch run kNN_twitch_benchmark
    args = parser.parse_args()
    if args.strat == "knn" and args.input == "twitch":
        kNN_twitch_benchmark()
    elif args.strat == "knn" and args.input == "amazon_fraud":
        kNN_fraud_benchmark()
    elif args.strat == "Graph":
        graph_benchmark()
    else:
        print("Invalid arguments. Exiting...")
        return
    
if __name__ == "__main__":
    main()