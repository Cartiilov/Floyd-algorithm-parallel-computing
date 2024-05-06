import subprocess
import os
import random

print("I was executed!")

def remove_whitespace(text, n):
    # lines = text.split("\n")
    # processed_lines = lines[:n]  # Keep only the first n lines
    # return "\n".join(processed_lines)
    return ''.join(text.split())

def compare_files_in_result_folder(name, n):
    result_folder = '../data/result'
    matrix_txt_path = os.path.join(result_folder, name + '.txt')
    matrix_py_txt_path = os.path.join(result_folder, name + '_py.txt')

    if os.path.exists(matrix_txt_path) and os.path.exists(matrix_py_txt_path):
        with open(matrix_txt_path, 'r') as file1, open(matrix_py_txt_path, 'r') as file2:
            content1 = remove_whitespace(file1.read(), n)
            content2 = remove_whitespace(file2.read(), n)

            if content1 == content2:
                return True
            else:
                return False

def execute_mpi_c_program(name, n, k):
    k = min(n, k)
    mpi_command = ['mpiexec', '-f', '../nodes', '-n', str(k), './floyd', str(n), '../data/source/' + name + '.txt']
    output_directory = '../data/result'
    output_file_path = os.path.join(output_directory, name + '.txt')
    with open(output_file_path, 'w') as output_file:
        process = subprocess.Popen(mpi_command, stdout=output_file, stderr=subprocess.PIPE)
        process.wait()
        if process.returncode != 0:
            print("Error executing MPI C program:", process.stderr.read().decode())

def execute_another_python_script(python_script_path, name):
    python_command = ['python3', python_script_path, name]
    subprocess.run(python_command, check=True)

def save_matrix_to_file(matrix, filename):
    with open('../data/source/' + filename + '.txt', 'w') as file:
        for row in matrix:
            file.write(' '.join(map(str, row)) + '\n')

def generate_random_matrix(name, size, density=0.4, max_weight=20):
    adjacency_matrix = [[0] * size for _ in range(size)]

    for i in range(size):
        for j in range(size):
            if i != j and random.random() < density:
                weight = random.randint(1, max_weight)
                adjacency_matrix[i][j] = weight

    save_matrix_to_file(adjacency_matrix, name)

def count_occurrences_of_stud():
    filename = '../nodes'
    with open(filename, 'r') as file:
            text = file.read()
            occurrences = text.count('stud')
            return occurrences

if __name__ == "__main__":
    random.seed(10)
    clusters = count_occurrences_of_stud()
    n = random.randint(5, 20)
    python_script_path = "../py_floyd.py" 
    tries = 20
    correct = tries
    for i in range(tries):
        name = 'matrix_another' + str(i)
        generate_random_matrix(name, n)
        execute_mpi_c_program(name, n, clusters)
        execute_another_python_script(python_script_path, name)
        if not compare_files_in_result_folder(name, n):
            correct -= 1
        print(f'progress: {i+1}/{tries}', end='', flush=True)
        if (i + 1) != tries:
            print('\b' * len(f'progress: {i+1}/{tries}'), end='', flush=True)

    print(f'\n{correct}/{tries} were correct')

