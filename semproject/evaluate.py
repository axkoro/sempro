import subprocess
import os

def main():
    strat_choice = ""
   
    #ask wich evaluation to run 
    print("Which strategie would you like to evaluate? \n 1.knn \n 2.louvain \n 3.gnn")
    strat_input = input("Enter the number of the strategy you would like to evaluate: ")
    if strat_input == "1":
        print("You have chosen to evaluate the kNN strategy.")
        strat_choice = "knn"
    elif strat_input == "2":
        print("You have chosen to evaluate the Louvain strategy.")
        strat_choice = "louvain"
    elif strat_input == "3":
        print("You have chosen to evaluate the GNN strategy.")
        strat_choice = "gnn"

    #ask which input file to use
    print("Which input file would you like to use ?\n 1.twitch \n 2.amazon \n3.cora \n4.genius \n5.amazon_fraud")
    input_input = input("Enter the number of the input file you would like to use: ")
    if input_input == "1":
        print("You have chosen to use the twitch input file.")
        #if twitch.zip exists in ../data/output/knn dont run benchmark
        run = True
        if os.path.exists("../data/output/knn/twitch.zip"):
            run_bench = False
        if run_bench:
        #run the benchmarks.py file with the input twitch
            subprocess.run(["python", "benchmarks.py", "--strat",strat_choice,"--input", "twitch"], check=True,text=True)
             # Zip the output .txt file into a twitch.zip file
            try:
                subprocess.run(["zip","-j", "../data/output/knn/twitch.zip", "../data/output/knn/twitch_features.txt"], check=True)
                print("Zipped twitch output successfully")
            except subprocess.CalledProcessError as e:
                print(f"An error occurred while zipping the file: {e}")

        # Run the measure-quality.py script and print output in real-time
        result =subprocess.run("python ../extlibs/evaluation/measure-quality.py -i twitch -if ../data/input -ff ../data/output/knn -rf ../data/reference",shell=True,capture_output=True,text=True)
        print(result.stdout)
       
    

    if input_input == "2":
        print("You have chosen to use the amazon input file.")
        
    if input_input == "3":
        print("You have chosen to use the cora input file.")
       
    if input_input == "4":
        print("You have chosen to use the genius input file.")
       
    if input_input == "5":
        print("You have chosen to use the amazon_fraud input file.")
       
    


if __name__ == "__main__":
    main()