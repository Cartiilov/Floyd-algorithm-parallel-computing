#ifndef READ_AND_DISTRIBUTE_MATRIX
#define READ_AND_DISTRIBUTE_MATRIX

#include <stdio.h>
#include <mpi.h>


inline int number_of_rows(int id, int ntotal_rows, int p);
inline int owner(int j, int p, int nrows);

void read_and_distribute_matrix_byrows(char *filename, void ***matrix, void **matrix_storage, MPI_Datatype dtype, int *nrows, int *ncols, int *errval, MPI_Comm comm);
void print_matrix(void **matrix, int nrows, int ncols, MPI_Datatype dtype, FILE *stream);
void collect_and_print_matrix(void **matrix, MPI_Datatype dtype, int nrows, int ncols, MPI_Comm comm);

#endif

