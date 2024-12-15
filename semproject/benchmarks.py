import time
import semproject.graph as graph_module
import semproject.strats as strats_module

def graph_benchmark():
    print("Running Graph tests...")
    start_time = time.time()
    # Create a Graph instance
    edges_path = "../input/twitch/twitch_edges.txt"
    features_path = "../input/twitch/twitch_features.txt"
    graph = graph_module.Graph(edges_path,features_path)

    end_time = time.time()
    
    print(f"Graph operations completed in {end_time - start_time:.4f} seconds")

def kNN_twitch_benchmark():
    print("Running kNN twitch benchmark with depth 3...")
    
    # Create a Graph instance
    edges_path = "../input/twitch/twitch_edges.txt"
    features_path = "../input/twitch/twitch_features.txt"
    graph = graph_module.Graph(edges_path,features_path)
    
    # Create a KNNImputer instance
    knn_imputer = strats_module.KNNImputer(graph)
    knn_imputer.set_depth(3)
    
    # Run the KNNImputer
    start_time = time.time()
    knn_imputer.run()
    end_time = time.time()
    
    print(f"kNN twitch benchmark completed in {end_time - start_time:.4f} seconds")

def kNN_fraud_benchmark():
    print("Running kNN amazon_fraud benchmark with depth 3...")
    
    # Create a Graph instance
    edges_path = "../input/amazon_fraud/amazon_fraud_edges.txt"
    features_path = "../input/amazon_fraud/amazon_fraud_features.txt"
    graph = graph_module.Graph(edges_path,features_path)
    
    # Create a KNNImputer instance
    knn_imputer = strats_module.KNNImputer(graph)
    knn_imputer.set_depth(3)
    
    # Run the KNNImputer
    start_time = time.time()
    knn_imputer.run()
    end_time = time.time()
    
    print(f"kNN amazon_fraud benchmark completed in {end_time - start_time:.4f} seconds")
def main():
    print("Select a benchmark to run:")
    print("1. Graph")
    print("2. kNN twitch")
    print("3. kNN amazon_fraud")

    choice = input("Enter your choice: ")
    if choice == "1":
        graph_benchmark()
    elif choice == "2":
        kNN_twitch_benchmark()
    elif choice == "3":
        kNN_fraud_benchmark()
    else:
        print("Invalid choice. Exiting...")
        return
if __name__ == "__main__":
    main()