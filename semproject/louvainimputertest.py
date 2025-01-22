import semproject.louvainimputer as louvainimputer_module
import semproject.graph as graph_module

# Test function for the LouvainImputer

# Paths to test files
edges_file = "/Users/bshar/OneDrive/Desktop/Semester-Project/sempro/sempro/input/Gtests/edges_example.txt"
features_file = "/Users/bshar/OneDrive/Desktop/Semester-Project/sempro/sempro/input/Gtests/features2_example.txt"

# Create a graph from the test files
graph = graph_module.Graph(edges_file, features_file)
print("Graph created successfully!")
print(f"Number of nodes: {graph.get_num_nodes()}")
print(f"Number of features: {graph.get_num_features()}")

# Dummy community assignments
communities = [0, 0, 0, 1, 1]  # Example: Nodes 0, 1, 2 in community 0; 3, 4 in community 1
print("Communities assigned successfully!")
# Initialize the LouvainImputer
imputer = louvainimputer_module.LouvainImputer(graph, communities)
print("LouvainImputer initialized successfully!")

assert len(communities) == graph.get_num_nodes(), "Mismatch between number of nodes and community assignments!"

# Run the imputation
imputer.run()
print("Imputation completed successfully!")

# Print the imputed features for verification
print("Imputed Features:")
for node in range(graph.get_num_nodes()):
    features = [graph.get_feature(node, feature) for feature in range(graph.get_num_features())]
    print(f"Node {node}: {features}")

