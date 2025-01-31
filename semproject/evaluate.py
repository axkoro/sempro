import subprocess
import os
import sys
import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--all", action="store_true", help="Use all strategies and inputs")
    parser.add_argument("--strat", type=str, help="Strategy to evaluate (knn, louvain, deepwalk)")
    parser.add_argument("--input", type=str, help="Data set to run the strategy on")
    args = parser.parse_args()

    # if no args provided, fall back to interactive
    if len(sys.argv) == 1:
        strat_choice = get_user_choice_strategy()
        print("")
        data_set_choice = get_user_choice_data_set()
        print("")
        if data_set_choice == "amazon_fraud":
            print(f"{data_set_choice} can't be evaluated at the moment.\nIf you want to run the strategy without evaluation, use the 'benchmark' script instead.")
            return
        train_choice = get_user_choice_train()
        print("")
        run_one_strategy(strat_choice, data_set_choice, train_choice)
    else:
        # Non-interactive logic
        if args.all:
            all_strats = ["knn", "louvain", "deepwalk"]
            all_sets = ["twitch", "amazon", "corafull", "genius", "amazon_fraud", "github"]
            for s in all_strats:
                for ds in all_sets:
                    if ds == "amazon_fraud":
                        print(f"{ds} can't be evaluated at the moment.\nUse 'benchmark' script instead.\n")
                        continue
                    print("="*50)
                    run_one_strategy(s, ds, False, force_rerun=True)
                    print("="*50)
                    print("")
        else:
            if not args.strat or not args.input:
                print("Error: Must provide --strat and --input, or use --all.")
                sys.exit(1)
            if args.input == "amazon_fraud":
                print(f"{args.input} can't be evaluated at the moment.\nUse 'benchmark' script instead.")
                return
            # default train_choice to False
            run_one_strategy(args.strat, args.input, False)

def run_one_strategy(strat_choice, data_set_choice, train_choice, force_rerun=False):
    output_txt_path = f"./data/output/{strat_choice}/{data_set_choice}_features.txt"
    run_strat = True
    if force_rerun == False and os.path.exists(output_txt_path):
        choice = input(f"Output file already exists ({strat_choice}, {data_set_choice}). Rerun the strategy? [Y]/n: ").strip().lower()
        if choice not in ['y', 'yes', '']:
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
    print("Select imputation strategy to evaluate:\n 1. kNN \n 2. Louvain \n 3. deepwalk")
    try:
        strat_input = int(input("Enter number: "))
    except ValueError:
        print("Invalid choice")
        return

    strat_dict = {
        1: "knn",
        2: "louvain",
        3: "deepwalk"
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