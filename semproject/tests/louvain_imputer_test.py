import semproject._strats as strats_module
import semproject.graph as graph_module
import time

import os

edges_file = os.path.join(os.path.dirname(__file__), "../../data/input/unzipped/twitch_edges.txt")
features_file = os.path.join(os.path.dirname(__file__), "../../data/input/unzipped/twitch_features.txt")

graph = graph_module.GraphDouble(edges_file, features_file)
print("Graph created successfully!")

communities = [0, 0, 0, 1, 1]  # Example: Nodes 0, 1, 2 in community 0; 3, 4 in community 1
start_time = time.time()
imputer = strats_module.CommunityImputer(graph, communities)
print("CommunityImputer initialized successfully!")

imputer.run()
print("Imputation completed successfully!")
end_time = time.time()

elapsed_time = end_time - start_time
print(f"Imputer execution time: {elapsed_time:.4f} seconds")


