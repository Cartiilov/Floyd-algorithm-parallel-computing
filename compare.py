import subprocess
import os
import random

print("I was executed!")

def execute_mpi_c_program(name):

    mpi_command = ['mpiexec', '-f', '../nodes', '-n', '3', './floyd 5', '../' + name, '>>', '../data/result/' + name]
    subprocess.run(mpi_command, check=True)

def execute_another_python_script(python_script_path):
    # Execute the other Python script
    python_command = ['python3', python_script_path]
    subprocess.run(python_command, check=True)

if __name__ == "__main__":
    # random.seed(10)
    # n = random.randint(1, 20)
    c_program_command = "mpiexec -f ../nodes -n 3 ./floyd 5 ../matrix.txt >> ../data/result/matrix.txt"
    python_script_path = " ../script.py" 
    name = 'matrix.txt'
    execute_mpi_c_program(name)
    execute_another_python_script(python_script_path)
