import semproject._strats as strats_module
import semproject.graph as graph_module

edges_file = "data/test/louvain/edges_example.txt"
features_file = "data/test/louvain/features2_example.txt"

graph = graph_module.GraphDouble(edges_file, features_file)
print("Graph created successfully!")

communities = [0, 0, 0, 1, 1]  # Example: Nodes 0, 1, 2 in community 0; 3, 4 in community 1
imputer = strats_module.LouvainImputer(graph, communities)
print("LouvainImputer initialized successfully!")

imputer.run()
print("Imputation completed successfully!")

graph.print_features()

