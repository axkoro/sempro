# Imputing Missing Node Features on Graphs

This project contains implementations for various imputation strategies to reconstruct missing node features in graph data. This is done to enhance the quality of downstream tasks such as node classification or link prediction.

## Overview

Three imputation strategies are implemented:
- **Baseline Strategy - kNN**: Impute the missing feature of a given node using the average of this feature among neighbouring nodes.
- **Topology-Based Strategy - Community Average**: Detects communities using the [Louvain method](https://en.wikipedia.org/wiki/Louvain_method) and imputes missing values with the average computed within each community.
- **Learning-Based Strategy - Attributed DeepWalk**: Extends the [DeepWalk](https://arxiv.org/abs/1403.6652) algorithm by incorporating node features into the random walk process. The resulting embeddings capture both topology and feature information and are used for imputation. For more details, see [our Wiki](https://github.com/axkoro/sempro/wiki/Attributed-DeepWalk).

## Setup

### 1. Dependencies
- Python (3.x) and pip.

### 2. Installation

```bash
git clone https://github.com/axkoro/graph-impute.git
cd graph-impute
git submodule init
git submodule update
pip install .
```

### 3. Extras (optional)
Install additional functionalities (e.g., training, plotting, testing):

```bash
pip install .[extra]
```

Extra options include: *train_evaluate*, *plot*, *test*.

## Usage

To run one of the implemented strategies on a graph dataset (see [Input Format](#input-format) for requirements), use either the [CLI tool](#examples-cli) or the [Python API](#example-python-api).

### Examples: CLI

#### Using a Pre-configured Dataset

```bash
graph-impute --strategy knn --dataset amazon
```

Loads file paths from [datasets.yaml](#the-datasetsyaml-file) for the "amazon" dataset and applies kNN imputation.

#### Using Custom File Paths

```bash
graph-impute --strategy deepwalk --edges path/to/edges.txt --features path/to/features.txt
```

Specifies custom paths and applies DeepWalk imputation.

#### Enabling Evaluation, Timing, and Training

```bash
graph-impute --strategy community --dataset amazon --evaluate --time --train
```

Uses the "amazon" dataset, performs community-based imputation, evaluates quality, prints timing details, and trains a neural network for downstream evaluation.

### Example: Python API

You can also use the imputation tool as a Python library. For example:

```python
edges_path = "path/to/edges.txt"
features_path = "path/to/features.txt"
reference_path = "path/to/reference_features.txt"
output_path = "path/to/imputed_features.txt"

graph = Graph.load(edges_path, features_path, feature_type=float)
imputer = create_imputer("knn", graph, k=150, use_k_hops=False)

imputer.impute()
graph.save(output_path)

metrics = evaluate_imputed_features_file(
    imputed_features_path=output_path,
    reference_path=reference_path,
    input_features_path=features_path
)

print(metrics)
```

## Command-Line Arguments

### General Options
- **--strategy** (required): Imputation strategy. Options: knn, community, deepwalk.
- **--dataset**: Pre-configured dataset name (e.g., amazon). When provided, file paths for edges and features are loaded from datasets.yaml.
- **--edges** and **--features**: Custom file paths for edge and feature data.
- **--output**: Output file path for imputed features (Default: ./imputed_features.txt)
- **--feature-type**: Data type for features. Options: bool, int, float (Default: float)
- **--config**: [YAML file mapping](#the-datasetsyaml-file) dataset names to file paths (Default: ./datasets.yaml)

### The `datasets.yaml` file

> **Tip**: See the `datasets.yaml` file in the root folder of the repo for an example.

The `datasets.yaml` file maps dataset names to their corresponding file paths and other properties. For each dataset, it specifies:
- **feature_type**: The data type for features (bool, float, int).
- **edges**: Path to the file containing edge data.
- **features**: Path to the file containing feature data.
- **reference**: (Optional) Path to a reference features file for evaluation purposes.

### Evaluation Options
- **--evaluate**: Evaluate the quality of imputed features.
- **--reference**: Reference file for evaluation.
- **--train**: Train a neural network using the imputed data (requires --evaluate).
- **--time**: Print timing details for major processing steps.

### kNN Imputer Options
- **--k**: Number of topological neighbours to consider. (Default: 100)
- **--use-k-hop**: Use neighbours within k hops rather than simply the k closest nodes (Default: False)

### Community Imputer Options
- **--community-algorithm**: Community detection algorithm. (Default: "louvain")
- **--max-levels**: Maximum coarsening levels for Louvain (Default: 10)
- **--max-iterations**: Maximum iterations per coarsening level (Default: 50)
- **--tolerance**: Minimum modularity improvement to continue iterations (Default: 1e-4)

### DeepWalk Imputer Options
- **--no-edge-weights**: Skip edge weight preprocessing (Default: False)
- **--fusion-coefficient**: Fusion coefficient value (Default: 0.6)
- **--walk-length**: Length of each random walk (Default: 80)
- **--num-walks**: Walks per node (Default: 10)
- **--embedding-size**: Size of embedding vectors (Default: 128)
- **--context-window**: Context window size (Default: 10)
- **--num-negative-samples**: Negative samples per positive sample (Default: 10)
- **--smoothing-exponent**: Smoothing exponent (Default: 0.75)
- **--num-epochs**: Training epochs (Default: 1)
- **--learning-rate**: Optimizer learning rate (Default: 0.025)
- **--top-similar**: Most similar nodes to use for imputation (Default: 10)
- **--similarity-metric**: Metric for comparing embeddings (Default: "cosine")


## Input Format

> **Tip**: Refer to the example datasets in `data/input/` for both edge file and feature file formatting.


### Graph Assumptions
- **Graph Type**: Undirected/symmetric with no self-loops.
- **Node Numbering**: Nodes are numbered consecutively starting at 0. All nodes in the range must be present.

### Edges File
- Each line represents an undirected edge.
- The two nodes per edge are ordered descending (i.e. in the line `a b` representing an edge, it holds that `b < a`).
- All edges are sorted in ascending order (e.g., `2 0` comes before `3 0` and `2 1`).
- **Example**:

```
2 0
3 2
5 2
5 3
6 0
6 5
```

### Node Feature Data File

Each row corresponds to a node in the graph. Follow these guidelines:

1. **File Format & Newlines**
   - Plain text file using LF for newlines.
   - The last line must end with a newline.

2. **Row Ordering & Node IDs**
   - Rows are in strictly ascending order by node ID (node 0 first).

3. **Column Structure**
   - Node ID: The first field (integer).
   - Feature List: A comma-separated list with each feature separated by a comma and a space (`, `).
   - Missing features are indicated by a single `#`.
   - All nodes must have the same number of feature values.
   - Label: The last field (integer); required and cannot be missing.

4. **Punctuation & Spacing**
   - Do not include a trailing comma after the last feature value.

## Example Datasets

The directory `data/` contains modified versions of several datasets originally provided by [PyTorch Geometric](https://pytorch-geometric.readthedocs.io/en/latest/) and [DGL](https://www.dgl.ai). The modifications include reformatting and the simulation of missing features to test the imputation strategies. Below are the datasets included:

- **Twitch**: Modified version of the dataset as described in the [Twitch documentation](https://pytorch-geometric.readthedocs.io/en/2.6.0/generated/torch_geometric.datasets.Twitch.html#torch_geometric.datasets.Twitch).
- **Cora**: Adapted from the CitationFull dataset and referred to as "cora" in this repository. More details can be found in the [CitationFull documentation](https://pytorch-geometric.readthedocs.io/en/2.6.0/generated/torch_geometric.datasets.CitationFull.html#torch_geometric.datasets.CitationFull).
- **Amazon**: Modified version available based on the original dataset described in the [Amazon documentation](https://pytorch-geometric.readthedocs.io/en/2.6.0/generated/torch_geometric.datasets.Amazon.html#torch_geometric.datasets.Amazon).
- **GitHub**: Adaptation of the dataset as detailed in the [GitHub documentation](https://pytorch-geometric.readthedocs.io/en/2.6.0/generated/torch_geometric.datasets.GitHub.html#torch_geometric.datasets.GitHub).
- **Genius**: Modified version of the dataset from LINKXDataset, here referred to as "genius". Refer to the [LINKXDataset documentation](https://pytorch-geometric.readthedocs.io/en/2.6.0/generated/torch_geometric.datasets.LINKXDataset.html#torch_geometric.datasets.LINKXDataset) for the original dataset details.
- **FraudAmazon (DGL)**: Modified version of the dataset based on [DGL's FraudAmazonDataset](https://www.dgl.ai/dgl_docs/generated/dgl.data.FraudAmazonDataset.html#dgl.data.FraudAmazonDataset).

For each dataset, please consult the original documentation for complete licensing information and usage rights. The modifications in this repo (e.g., reformatting and simulating missing features) are provided for research and experimental purposes.

## Implementation Details & Contributing

see [our Wiki](https://github.com/axkoro/sempro/wiki)