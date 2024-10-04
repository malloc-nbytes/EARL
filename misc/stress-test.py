#!/bin/python3

import time
import subprocess
import os

def measure_average_runtime(command, iterations=7000):
    total_time = 0

    for i in range(iterations):
        start_time = time.time()
        subprocess.run(command, shell=True, check=True)
        end_time = time.time()

        runtime = end_time - start_time
        total_time += runtime

        if i >= 10 and (i + 1) % (iterations // 10) == 0:
            print(f"Progress: {i + 1} / {iterations}")

    average_runtime = total_time / iterations
    print(f"Average runtime over {iterations} iterations: {average_runtime:.6f} seconds")

if __name__ == "__main__":
    print("current:", os.getcwd())
    new_directory = './src/test/earl-tests/'
    try:
        os.chdir(new_directory)
        print("Directory changed successfully.")
    except FileNotFoundError as e:
        print(f"Error: {e}")
    command_to_run = '../../../build/earl ./main.earl'
    measure_average_runtime(command_to_run)

