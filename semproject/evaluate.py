import subprocess
import os

def main():
    twitch_output_path ="../data/output/knn/twitch.zip"
    print("Enter Evaluation parameters")
   
    #run the benchmarks.py file with the input twitch
    subprocess.run(["python", "benchmarks.py", "--strat","knn","--input","twitch"], check=True,text=True)
   
    # Zip the output .txt file into a twitch.zip file
    try:
        subprocess.run(["zip","-j", twitch_output_path, "../data/output/knn/twitch_features.txt"], check=True)
        print("Zipped twitch output successfully")
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while zipping the file: {e}")
     # Run the measure-quality.py script and print output in real-time
    result =subprocess.run("python ../extlibs/evaluation/measure-quality.py -i twitch -if ../data/input -ff ../data/output/knn -rf ../data/reference",shell=True,capture_output=True,text=True)
    print(result.stdout)

if __name__ == "__main__":
    main()