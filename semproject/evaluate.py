import subprocess
import os

def main():
    twitch_input = "../data/input/twitch.zip"
    twitch_output ="../data/output/twitch.zip"
    twitch_reference = "twitch_output.txt"
    print("Enter Evaluation parameters")
#run benchmarks.py as subprocess
    env = os.environ.copy()
    env["PYTHONUNBUFFERED"] = "1"

    process = subprocess.Popen("python benchmarks.py --strat knn --input twitch",stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True,bufsize=1,shell=True,universal_newlines=True,env=env)
    for line in process.stdout:
        print(line, end='',flush=True)
    process.wait()

        # Check for errors
    if process.returncode != 0:
        print(f"An error occurred while running the subprocess: {process.stderr.read()}")

    
if __name__ == "__main__":
    main()