import time
import semproject.graph as graph_module
import semproject.strats as strats_module

def run_graph_tests():
    print("Running Graph tests...")
    start_time = time.time()
    # Create a Graph instance
    edges_path = "../input/twitch/twitch_edges.txt"
    features_path = "../input/twitch/twitch_features.txt"
    graph = graph_module.Graph(edges_path,features_path)

    end_time = time.time()
    
    print(f"Graph operations completed in {end_time - start_time:.4f} seconds")

def run_knn_imputer_tests():
    print("Running KNNImputer tests...")
    
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
    
    print(f"KNNImputer run completed in {end_time - start_time:.4f} seconds")

if __name__ == "__main__":
    run_graph_tests()
    run_knn_imputer_tests()