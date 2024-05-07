#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "floyd.h"


int main(int argc, char *argv[]) {
    int** M;
    int* storg;
    int id;
    int m;
    int n;
    int p;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    size = atoi(argv[1]);
    if (p > size) {
        printf("Number of nodes greater than number of matrix rows - terminating");
        MPI_Abort(MPI_COMM_WORLD, -1);
        MPI_Finalize();
        return -1;
    } else if (argc < 3) {
        printf("Insufficient number of command-line arguments - terminating");
        MPI_Abort(MPI_COMM_WORLD, -1);
        MPI_Finalize();
        return -1;
    }

    distribute_matrix(argv[2], (void *)&M, (void *)&storg, &m, &n, size);

    MPI_Barrier(MPI_COMM_WORLD);
    floyd(id, p, (int **)M, n);

    assemble_and_print_matrix((void **)M, m, n);

    MPI_Finalize();
    return 0;
}
