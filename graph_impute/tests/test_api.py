import os
import tempfile

from graph_impute.graph import Graph
from graph_impute.imputation import (
    CommunityImputer,
    DeepWalkImputer,
    KNNImputer,
    create_imputer,
)


def create_dummy_files():
    edges_file = tempfile.NamedTemporaryFile(delete=False, mode="w", suffix=".txt")
    features_file = tempfile.NamedTemporaryFile(delete=False, mode="w", suffix=".txt")

    edges_file.write("""1 0\n
                     2 1\n
                     3 2\n""")
    features_file.write("""0\t0.1, 0.2\t4\n
                        1\t#, 0.4\t4\n
                        2\t0.5, #\t-2\n
                        3\t0.7, 0.8\t0\n""")

    edges_file.close()
    features_file.close()
    return edges_file.name, features_file.name


def remove_dummy_files(*files):
    for f in files:
        try:
            os.remove(f)
        except OSError:
            pass


def test_graph_load_and_save():
    edges_file, features_file = create_dummy_files()
    try:
        graph = Graph.load(edges_file, features_file, float)

        save_file = tempfile.NamedTemporaryFile(delete=False).name
        graph.save(save_file)

        assert os.path.exists(save_file)
        with open(save_file) as f:
            content = f.read()
        assert content.strip() != ""
        os.remove(save_file)
    finally:
        remove_dummy_files(edges_file, features_file)


def test_knn_imputer():
    edges_file, features_file = create_dummy_files()
    try:
        graph = Graph.load(edges_file, features_file, float)
        imputer = KNNImputer(graph, depth=3)
        imputer.impute()
    finally:
        remove_dummy_files(edges_file, features_file)


def test_community_imputer():
    edges_file, features_file = create_dummy_files()
    try:
        graph = Graph.load(edges_file, features_file, float)
        imputer = CommunityImputer(graph, community_algorithm="louvain")
        imputer.impute()
    finally:
        remove_dummy_files(edges_file, features_file)


def test_deepwalk_imputer():
    edges_file, features_file = create_dummy_files()
    try:
        graph = Graph.load(edges_file, features_file, float)
        imputer = DeepWalkImputer(
            graph,
            fusion_coefficient=0.7,
            walk_length=50,
            num_walks=12,
            embedding_size=256,
            context_window=8,
            num_negative_samples=15,
            smoothing_exponent=0.8,
            num_epochs=10,
            learning_rate=0.01,
        )
        imputer.impute()
    finally:
        remove_dummy_files(edges_file, features_file)


def test_create_imputer():
    edges_file, features_file = create_dummy_files()
    try:
        graph = Graph.load(edges_file, features_file, float)
        knn = create_imputer("knn", graph, depth=3)
        community = create_imputer("community", graph, community_algorithm="louvain")
        deepwalk = create_imputer("deepwalk", graph, num_epochs=2)

        assert isinstance(knn, KNNImputer)
        assert isinstance(community, CommunityImputer)
        assert isinstance(deepwalk, DeepWalkImputer)
    finally:
        remove_dummy_files(edges_file, features_file)
