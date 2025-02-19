import time
from pathlib import Path

from graph_impute.evaluation import evaluate_imputed_graph
from graph_impute.graph import Graph
from graph_impute.imputation import create_imputer


def test_k_hop_k_nearest():
    current_file = Path(__file__).resolve()
    project_root = current_file.parent.parent.parent

    edges_path = project_root / "data" / "input" / "amazon_edges.txt"
    features_path = project_root / "data" / "input" / "amazon_features.txt"
    reference_path = project_root / "data" / "reference" / "amazon_features.txt"

    g1 = Graph.load(str(edges_path), str(features_path), bool)
    knn_nearest = create_imputer("knn", g1, use_k_hop=False, k=100)
    start = time.time()
    knn_nearest.impute()
    end = time.time()
    print(end - start)
    print(evaluate_imputed_graph(g1, features_path, reference_path))

    g2 = Graph.load(str(edges_path), str(features_path), bool)
    knn_hops = create_imputer("knn", g2, use_k_hop=True, k=2)
    start = time.time()
    knn_hops.impute()
    end = time.time()
    print(end - start)
    print(evaluate_imputed_graph(g2, features_path, reference_path))


def main():
    test_k_hop_k_nearest()


if __name__ == "__main__":
    main()
