import subprocess
import os
import sys

def main():
    strat_choice = get_user_choice_strategy()
    print(strat_choice + " was chosen.\n")

    data_set_choice = get_user_choice_data_set()
    print(data_set_choice + " was chosen.\n")

    # Run the strategy
    output_txt_path = "../data/output/" + strat_choice + "/" + data_set_choice + "_features.txt"

    run_strat = True
    if os.path.exists(output_txt_path):
        choice = input("Output files already exists. Rerun the strategy? [Y]/n: ").strip().lower()
        if choice in ['y', 'yes', '']:
            run_strat = True
        else:
            run_strat = False
            print("Using existing output files for evaluation.")


    if run_strat:
        print("")
        subprocess.run([sys.executable, "benchmarks.py", "--strat", strat_choice, "--input", data_set_choice], check=True, text=True)
        
    # Run the measure-quality.py script and print output in real-time
    print("")
    result = subprocess.run("python ../extlibs/evaluation/measure-quality.py -i github -if ../data/input -ff ../data/output/knn -rf ../data/reference",shell=True,capture_output=True,text=True)
    print(result.stdout)

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
        3: "cora",
        4: "genius",
        5: "amazon_fraud",
        6: "github"
    }

    if data_set_input not in data_set_dict:
        print("Invalid choice")
        return
    
    return data_set_dict[data_set_input]

if __name__ == "__main__":
    main()