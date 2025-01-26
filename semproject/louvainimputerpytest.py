import semproject._louvainimputer as louvainimputer_module
import semproject.graph as graph_module

# Paths to test files
edges_file = "/Users/bshar/OneDrive/Desktop/Semester-Project/sempro/sempro/input/Gtests/edges_example.txt"
features_file = "/Users/bshar/OneDrive/Desktop/Semester-Project/sempro/sempro/input/Gtests/features2_example.txt"

# Create a graph from the test files
graph = graph_module.GraphDouble(edges_file, features_file)  # Use the appropriate graph type
print("Graph created successfully!")
print(f"Number of nodes: {graph.get_num_nodes()}")
print(f"Number of features: {graph.get_num_features()}")

# Dummy community assignments
communities = [0, 0, 0, 1, 1]  # Example: Nodes 0, 1, 2 in community 0; 3, 4 in community 1
print("Communities assigned successfully!")

# Initialize the LouvainImputer
imputer = louvainimputer_module.LouvainImputer(graph, communities)
print("LouvainImputer initialized successfully!")

# Run the imputation
imputer.run()
print("Imputation completed successfully!")