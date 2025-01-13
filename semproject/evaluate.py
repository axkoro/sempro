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
        print("Louvain strategy not implemented yet.")
        return
    elif strat_input == "3":
        print("You have chosen to evaluate the GNN strategy.")
        strat_choice = "gnn"
        print("GNN strategy not implemented yet.")
        return

    #ask which input file to use
    print("Which input file would you like to use ?\n 1.twitch \n 2.amazon \n 3.cora \n 4.genius \n 5.amazon_fraud \n 6.github") 
    input_input = input("Enter the number of the input file you would like to use: ")
    if input_input == "1":
        print("You have chosen to use the twitch input file.")
        #if twitch.zip exists in ../data/output/knn dont run benchmark
        run_bench = True
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
        
        #if amazon.zip exists in ../data/output/knn dont run benchmark
        run_bench = True
        if os.path.exists("../data/output/knn/amazon.zip"):
            run_bench = False
        if run_bench:
        #run the benchmarks.py file with the input twitch
            subprocess.run(["python", "benchmarks.py", "--strat",strat_choice,"--input", "amazon"], check=True,text=True)
             # Zip the output .txt file into a twitch.zip file
            try:
                subprocess.run(["zip","-j", "../data/output/knn/amazon.zip", "../data/output/knn/amazon_features.txt"], check=True)
                print("Zipped amazon output successfully")
            except subprocess.CalledProcessError as e:
                print(f"An error occurred while zipping the file: {e}")

        # Run the measure-quality.py script and print output in real-time
        result =subprocess.run("python ../extlibs/evaluation/measure-quality.py -i amazon -if ../data/input -ff ../data/output/knn -rf ../data/reference",shell=True,capture_output=True,text=True)
        print(result.stdout)
    if input_input == "3":
        print("You have chosen to use the cora input file.")
        print("Cora is still in development.")
        return
        #if cora.zip exists in ../data/output/knn dont run benchmark
        run_bench = True
        if os.path.exists("../data/output/knn/cora_full.zip"):
            run_bench = False
        if run_bench:
        #run the benchmarks.py file with the input twitch
            subprocess.run(["python", "benchmarks.py", "--strat",strat_choice,"--input", "cora_full"], check=True,text=True)
             # Zip the output .txt file into a twitch.zip file
            try:
                subprocess.run(["zip","-j", "../data/output/knn/cora_full.zip", "../data/output/knn/corafull_features.txt"], check=True)
                print("Zipped cora_full output successfully")
            except subprocess.CalledProcessError as e:
                print(f"An error occurred while zipping the file: {e}")

        # Run the measure-quality.py script and print output in real-time
        result =subprocess.run("python ../extlibs/evaluation/measure-quality.py -i cora -if ../data/input -ff ../data/output/knn -rf ../data/reference",shell=True,capture_output=True,text=True)
        print(result.stdout)
    
    if input_input == "4":
        print("You have chosen to use the genius input file.")
        
        #if genius.zip exists in ../data/output/knn dont run benchmark
        run_bench = True
        if os.path.exists("../data/output/knn/genius.zip"):
            run_bench = False
        if run_bench:
        #run the benchmarks.py file with the input twitch
            subprocess.run(["python", "benchmarks.py", "--strat",strat_choice,"--input", "genius"], check=True,text=True)
             # Zip the output .txt file into a twitch.zip file
            try:
                subprocess.run(["zip","-j", "../data/output/knn/genius.zip", "../data/output/knn/genius_features.txt"], check=True)
                print("Zipped genius output successfully")
            except subprocess.CalledProcessError as e:
                print(f"An error occurred while zipping the file: {e}")

        # Run the measure-quality.py script and print output in real-time
        result =subprocess.run("python ../extlibs/evaluation/measure-quality.py -i genius -if ../data/input -ff ../data/output/knn -rf ../data/reference",shell=True,capture_output=True,text=True)
        print(result.stdout)

    if input_input == "5":
        print("You have chosen to use the amazon_fraud input file.")
        print("Amazon_fraud evaluation is not working yet.")
        return
        #if amazon_fraud.zip exists in ../data/output/knn dont run benchmark
        run_bench = True
        if os.path.exists("../data/output/knn/amazon_fraud.zip"):
            run_bench = False
        if run_bench:
        #run the benchmarks.py file with the input twitch
            subprocess.run(["python", "benchmarks.py", "--strat",strat_choice,"--input", "amazon_fraud"], check=True,text=True)
             # Zip the output .txt file into a twitch.zip file
            try:
                subprocess.run(["zip","-j", "../data/output/knn/amazon_fraud.zip", "../data/output/knn/amazonfraud_features.txt"], check=True)
                print("Zipped amazon_fraud output successfully")
            except subprocess.CalledProcessError as e:
                print(f"An error occurred while zipping the file: {e}")

        # Run the measure-quality.py script and print output in real-time
        result =subprocess.run("python ../extlibs/evaluation/measure-quality.py -i amazon_fraud -if ../data/input -ff ../data/output/knn -rf ../data/reference",shell=True,capture_output=True,text=True)
        print(result.stdout)
    if input_input == "6":
        print("You have chosen to use the github input file.")
        print("Github is still in development.")
        return
        #if github.zip exists in ../data/output/knn dont run benchmark
        run_bench = True
        if os.path.exists("../data/output/knn/github.zip"):
            run_bench = False
        if run_bench:
        #run the benchmarks.py file with the input twitch
            subprocess.run(["python", "benchmarks.py", "--strat",strat_choice,"--input", "github"], check=True,text=True)
             # Zip the output .txt file into a twitch.zip file
            try:
                subprocess.run(["zip","-j", "../data/output/knn/github.zip", "../data/output/knn/github_features.txt"], check=True)
                print("Zipped github output successfully")
            except subprocess.CalledProcessError as e:
                print(f"An error occurred while zipping the file: {e}")

        # Run the measure-quality.py script and print output in real-time
        result =subprocess.run("python ../extlibs/evaluation/measure-quality.py -i github -if ../data/input -ff ../data/output/knn -rf ../data/reference",shell=True,capture_output=True,text=True)
        print(result.stdout)


if __name__ == "__main__":
    main()