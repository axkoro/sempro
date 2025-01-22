import subprocess
import os
import sys

def main():
    strat_choice = get_user_choice_strategy()
    print("")

    data_set_choice = get_user_choice_data_set()
    print("")

    if data_set_choice == "amazon_fraud":
        print(f"{data_set_choice} can't be evaluated at the moment.\nIf you want to run the strategy without evaluation, use the 'benchmark' script instead.")
        return

    train_choice = get_user_choice_train()
    print("")

    # Run the strategy
    output_txt_path = f"./data/output/{strat_choice}/{data_set_choice}_features.txt"

    run_strat = True
    if os.path.exists(output_txt_path):
        choice = input("Output file already exists. Rerun the strategy? [Y]/n: ").strip().lower()
        if choice in ['y', 'yes', '']:
            run_strat = True
        else:
            run_strat = False
            print("Using existing output file for evaluation.")
        print("")

    if run_strat:
        subprocess.run(["benchmark", "--strat", strat_choice, "--input", data_set_choice], check=True, text=True)
        print("")
        
    evaluation_cmd = [
        sys.executable,
        "./extlibs/evaluation/measure-quality.py",
        "--instance", data_set_choice,
        "--input-folder", "./data/input",
        "--feature-folder", f"./data/output/{strat_choice}",
        "--reference-folder", "./data/reference",
        "--txt"
    ]
    if train_choice:
        evaluation_cmd.append("--train")

    process = subprocess.run(
        evaluation_cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        text=True
    )
    for line in process.stdout:
        print(line, flush=True, end='')

def get_user_choice_strategy():
    print("Select imputation strategy to evaluate:\n 1. kNN \n 2. Louvain \n 3. GNN")
    try:
        strat_input = int(input("Enter number: "))
    except ValueError:
        print("Invalid choice")
        return

    strat_dict = {
        1: "knn",
        2: "louvain",
        3: "gnn"
    }

    if strat_input not in strat_dict:
        print("Invalid choice")
        return

    return strat_dict[strat_input]

def get_user_choice_data_set():
    print("Select data set to run the strategy on:\n 1. twitch\n 2. amazon\n 3. cora\n 4. genius\n 5. amazon_fraud\n 6. github")
    try:
        data_set_input = int(input("Enter number: "))
    except ValueError:
        print("Invalid choice")
        return

    data_set_dict = {
        1: "twitch",
        2: "amazon",
        3: "corafull",
        4: "genius",
        5: "amazon_fraud",
        6: "github"
    }

    if data_set_input not in data_set_dict:
        print("Invalid choice")
        return

    return data_set_dict[data_set_input]

def get_user_choice_train():
    choice = input("Additionally evaluate the output using downstream tasks (train a neural network)? y/[N]: ").strip().lower()
    if choice in ['n', 'no', '']:
        return False
    else:
        return True

if __name__ == "__main__":
    main()