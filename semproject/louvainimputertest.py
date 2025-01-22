import semproject._louvainimputer as louvainimputer_module
import semproject.graph as graph_module

# Paths to test files (Change these paths to the test files in your local machine)
edges_file = "/Users/bshar/OneDrive/Desktop/Semester-Project/sempro/sempro/input/Gtests/edges_example.txt"
features_file = "/Users/bshar/OneDrive/Desktop/Semester-Project/sempro/sempro/input/Gtests/features2_example.txt"


# Create a graph from the test files
graph = graph_module.Graph(edges_file, features_file)
print("Graph created successfully!")

# Dummy community assignments
communities = [0, 0, 0, 1, 1]  # Example: Nodes 0, 1, 2 in community 0; 3, 4 in community 1
print("Communities assigned successfully!")

# Initialize the LouvainImputer
imputer = louvainimputer_module.LouvainImputer(graph, communities)
print("LouvainImputer initialized successfully!")


# Run the imputation
imputer.run()
print("Imputation completed successfully!")

graph.print_features()

