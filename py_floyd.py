import numpy as np
import sys
import os


def read_matrix_from_file(filename):
    filepath = "../data/source/" + filename + ".txt"
    with open(filepath, "r") as f:
        all_lines = [list(map(int, a.replace('\n', '').split(' ')))
                     for a in f.readlines()]
        return all_lines
    
def matrix_to_edge_list(matrix):
    edge_list = []
    num_rows, num_cols = matrix.shape 

    for i in range(num_rows):
        for j in range(num_cols):
            weight = matrix[i][j]
            if weight != 0:
                edge_list.append((i, j, weight))

    return edge_list

def save_matrix_to_file(A, filename):
    filepath = "../data/result/" + filename + "_py.txt"
    if not os.path.exists(filepath):
        open(filepath, 'w')

    with open(filepath,"w") as f:
        for i in range(len(A)):
            s = ""
            for j in range(len(A[i])):
                s+=f'{A[i,j]} '
            f.writelines(f'{s[:-1]}\n')

def save_path_to_file(path, filepath="../path_diff.txt"):
    with open(filepath,"w") as f:
        for p in path:
            f.write(f'{p} ')

def construct_path(prev, u,v):
    if prev[u, v] == -1:
        return []
    path = [v]
    while u != v:
        v = prev[u,v]
        path = [v, *path]
    return path

if __name__ == "__main__":

    if len(sys.argv) < 2:
        print('This python program accepts one argument - the name of the file from which to read the adjacency matrix\n and the name of the file to write results to')
    filename = sys.argv[1]
    matrix = np.array(read_matrix_from_file(filename))
    A = np.copy(matrix)
    edge_list = matrix_to_edge_list(matrix)
    prev = np.full_like(A, -1)
    n = len(matrix)

    for edge in edge_list:
        prev[edge[0], edge[1]] = edge[0]

    for i in range(n):
        prev[i,i] = i + 1

    for k in range(0, n):
        for i in range(0, n):
            for j in range(0, n):
                if A[i, j] > A[i, k] + A[k, j]:
                    A[i, j] = A[i, k] + A[k, j]
                    prev[i, j] = prev[k, j]
    # print(A)

    save_matrix_to_file(A, filename)
    # save_path_to_file(construct_path(prev, 0, 2))




