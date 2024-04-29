#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "utilities.h"
#include "matrix_io.h"

typedef int Element_type;
#define MPI_TYPE MPI_INT

void compute_shortest_paths(int id, int p, Element_type **a, int n, MPI_Comm comm);

int main(int argc, char *argv[])
{
    Element_type **adjmatrix;
    Element_type *matrix_storage;
    int id;
    int nrows;
    int ncols;
    int p;
    double time;
    double max_time;
    double total_time = 0;
    int error;
    char errstring[127];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (argc < 2)
    {
        sprintf(errstring, "Usage: %s filename, where filename contains binary adjacency matrix\n", argv[0]);
        terminate(id, errstring);
    }

    read_and_distribute_matrix_byrows(argv[1],
                                      (void **)&adjmatrix,
                                      (void **)&matrix_storage,
                                      MPI_TYPE, &nrows, &ncols, &error,
                                      MPI_COMM_WORLD);

    if (SUCCESS != error)
    {
        terminate(id, "Error reading or allocating matrix.\n");
    }

    if (nrows != ncols)
    {
        terminate(id, "Error: matrix is not square.\n");
    }

    collect_and_print_matrix((void **)adjmatrix, MPI_TYPE, nrows, ncols,
                             MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    time = -MPI_Wtime();

    compute_shortest_paths(id, p, (Element_type **)adjmatrix, ncols,
                           MPI_COMM_WORLD);

    time += MPI_Wtime();

    MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);
    MPI_Reduce(&time, &total_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (0 == id)
        printf("Floyd, matrix size %d, %d processes. Elapsed time %6.4f seconds, Total time %6.4f seconds\n",
               ncols, p, max_time, total_time);

    collect_and_print_matrix((void**)adjmatrix, MPI_TYPE, nrows, ncols,
                             MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}


void compute_shortest_paths(int id, int p, Element_type **a, int n, MPI_Comm comm)
{
    int i, j, k;
    int local_index;
    int root;
    Element_type *tmp;
    int nlocal_rows;

    tmp = (Element_type *)malloc(n * sizeof(Element_type));

    for (k = 0; k < n; k++)
    {
        root = owner(k, p, n);

        if (root == id)
        {
            local_index = k - (id * n) / p;

            for (j = 0; j < n; j++)
                tmp[j] = a[local_index][j];
        }

        MPI_Bcast(tmp, n, MPI_TYPE, root, comm);

        nlocal_rows = number_of_rows(id, n, p);

        for (i = 0; i < nlocal_rows; i++)
            for (j = 0; j < n; j++)
                a[i][j] = MIN(a[i][j], a[i][k] + tmp[j]);
    }

    free(tmp);
}