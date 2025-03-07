import copy

import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch_geometric.datasets import Amazon, CitationFull, GitHub, LINKXDataset, Twitch
from torch_geometric.loader import DataLoader
from torch_geometric.nn import GATv2Conv, GCNConv, SAGEConv
from tqdm import trange


class GNNStack(torch.nn.Module):
    def __init__(self, input_dim, hidden_dim, output_dim, args, emb=False):
        super(GNNStack, self).__init__()
        # conv_model = GATv2Conv
        # conv_model = GCNConv
        conv_model = SAGEConv

        self.convs = nn.ModuleList()
        self.convs.append(conv_model(input_dim, hidden_dim))
        assert args.num_layers >= 1, "Number of layers is not >=1"
        for l in range(args.num_layers - 1):
            self.convs.append(conv_model(args.heads * hidden_dim, hidden_dim))

        # post-message-passing
        self.post_mp = nn.Sequential(
            nn.Linear(args.heads * hidden_dim, hidden_dim),
            nn.Dropout(args.dropout),
            nn.Linear(hidden_dim, output_dim),
        )

        self.dropout = args.dropout
        self.num_layers = args.num_layers

        self.emb = emb

    def forward(self, data):
        x, edge_index, batch = data.x, data.edge_index, data.batch

        for i in range(self.num_layers):
            x = self.convs[i](x, edge_index)
            x = F.relu(x)
            x = F.dropout(x, p=self.dropout, training=self.training)

        x = self.post_mp(x)

        if self.emb == True:
            return x

        return F.log_softmax(x, dim=1)

    def loss(self, pred, label):
        return F.nll_loss(pred, label)


def build_optimizer(args, params):
    weight_decay = args.weight_decay
    filter_fn = filter(lambda p: p.requires_grad, params)
    if args.opt == "adam":
        optimizer = optim.Adam(filter_fn, lr=args.lr, weight_decay=weight_decay)
    elif args.opt == "sgd":
        optimizer = optim.SGD(filter_fn, lr=args.lr, momentum=0.95, weight_decay=weight_decay)
    elif args.opt == "rmsprop":
        optimizer = optim.RMSprop(filter_fn, lr=args.lr, weight_decay=weight_decay)
    elif args.opt == "adagrad":
        optimizer = optim.Adagrad(filter_fn, lr=args.lr, weight_decay=weight_decay)
    if args.opt_scheduler == "none":
        return None, optimizer
    elif args.opt_scheduler == "step":
        scheduler = optim.lr_scheduler.StepLR(
            optimizer, step_size=args.opt_decay_step, gamma=args.opt_decay_rate
        )
    elif args.opt_scheduler == "cos":
        scheduler = optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=args.opt_restart)
    return scheduler, optimizer


def train(dataset, args):
    test_loader = loader = DataLoader(dataset, batch_size=args.batch_size, shuffle=False)

    # build model
    model = GNNStack(dataset.num_node_features, args.hidden_dim, dataset.num_classes, args)
    scheduler, opt = build_optimizer(args, model.parameters())

    # train
    losses = []
    test_accs = []
    best_acc = 0
    best_model = None
    for epoch in trange(args.epochs, desc="Training", unit="Epochs"):
        total_loss = 0
        model.train()
        for batch in loader:
            opt.zero_grad()
            pred = model(batch)
            label = batch.y
            pred = pred[batch.train_mask]
            label = label[batch.train_mask]
            loss = model.loss(pred, label)
            loss.backward()
            opt.step()
            total_loss += loss.item() * batch.num_graphs
        total_loss /= len(loader.dataset)
        losses.append(total_loss)

        if epoch % 10 == 0:
            test_acc = test(test_loader, model)
            test_accs.append(test_acc)
            if test_acc > best_acc:
                best_acc = test_acc
                best_model = copy.deepcopy(model)
        else:
            test_accs.append(test_accs[-1])

    return test_accs, losses, best_model, best_acc, test_loader


def test(loader, test_model, is_validation=False):
    test_model.eval()

    correct = 0

    for data in loader:
        with torch.no_grad():
            # max(dim=1) returns values, indices tuple; only need indices
            pred = test_model(data).max(dim=1)[1]
            label = data.y

        mask = data.val_mask if is_validation else data.test_mask
        # node classification: only evaluate on nodes in test set
        pred = pred[mask]
        label = label[mask]
        correct += pred.eq(label).sum().item()

    total = 0
    for data in loader.dataset:
        total += torch.sum(data.val_mask if is_validation else data.test_mask).item()

    return correct / total


class objectview(object):
    def __init__(self, d):
        self.__dict__ = d


def train_and_evaluate_gnn_with_imputation(dataset_name: str, recon_features):
    args_gnn = objectview(
        {
            "num_layers": 2,
            "heads": 1,
            "batch_size": 32,
            "hidden_dim": 64,
            "dropout": 0.5,
            "epochs": 500,
            "opt": "adam",
            "opt_scheduler": "none",
            "opt_restart": 0,
            "weight_decay": 5e-3,
            "lr": 0.01,
        }
    )

    if dataset_name == "twitch":
        dataset = Twitch(root="./download/twitch", name="DE")
    elif dataset_name == "cora":
        dataset = CitationFull(root="./download/corafull", name="cora")
    elif dataset_name == "amazon":
        dataset = Amazon(root="./download/amazon", name="Computers")
    elif dataset_name == "github":
        dataset = GitHub(root="./download/github")
    elif dataset_name == "genius":
        dataset = LINKXDataset(root="./download/genius", name="genius")
    else:
        raise ValueError(f"Can't train using dataset '{dataset_name}'.")

    # print("downloaded")

    dset_copy = copy.deepcopy(dataset)
    data = dataset[0]

    # Number of nodes in the graph
    num_nodes = dataset[0].num_nodes

    # Total indices for all nodes
    indices = torch.randperm(num_nodes)

    # Define the percentage for train, validation, and test splits
    train_size = int(0.7 * num_nodes)  # 70% for training
    val_size = int(0.1 * num_nodes)  # 10% for validation
    test_size = num_nodes - train_size - val_size  # Remaining 20% for testing

    # Split indices for train, validation, and test
    train_indices = indices[:train_size]
    val_indices = indices[train_size : train_size + val_size]
    test_indices = indices[train_size + val_size :]

    # Create masks
    data["train_mask"] = torch.zeros(num_nodes, dtype=torch.bool)
    data["val_mask"] = torch.zeros(num_nodes, dtype=torch.bool)
    data["test_mask"] = torch.zeros(num_nodes, dtype=torch.bool)
    data.train_mask[train_indices] = True
    data.val_mask[val_indices] = True
    data.test_mask[test_indices] = True
    dset_copy.data = data

    # print("starting training")
    test_accs, losses, best_model, best_acc, test_loader = train(dset_copy, args_gnn)
    # print("finished training")

    # print("Maximum test set accuracy: {0}".format(max(test_accs)))
    # print("Minimum loss: {0}".format(min(losses)))

    # Run test for our best model
    # print("starting testing")
    model_accuracy_original = test(test_loader, best_model, is_validation=True)
    # print("finished testing")

    # Now exchange original for imputed features
    data["x"] = torch.Tensor(recon_features)

    # Total indices for all nodes
    indices = torch.randperm(num_nodes)

    # Define the percentage for train, validation, and test splits
    train_size = int(0.7 * num_nodes)  # 70% for training
    val_size = int(0.1 * num_nodes)  # 10% for validation
    test_size = num_nodes - train_size - val_size  # Remaining 20% for testing

    # Split indices for train, validation, and test
    train_indices = indices[:train_size]
    val_indices = indices[train_size : train_size + val_size]
    test_indices = indices[train_size + val_size :]

    # Create masks
    data["train_mask"] = torch.zeros(num_nodes, dtype=torch.bool)
    data["val_mask"] = torch.zeros(num_nodes, dtype=torch.bool)
    data["test_mask"] = torch.zeros(num_nodes, dtype=torch.bool)
    data.train_mask[train_indices] = True
    data.val_mask[val_indices] = True
    data.test_mask[test_indices] = True
    dset_copy.data = data

    test_accs, losses, best_model, best_acc, test_loader = train(dset_copy, args_gnn)

    # print("Maximum test set accuracy: {0}".format(max(test_accs)))
    # print("Minimum loss: {0}".format(min(losses)))

    # Run test for our best model
    model_accuracy_imputed = test(test_loader, best_model, is_validation=True)

    return model_accuracy_original, model_accuracy_imputed
