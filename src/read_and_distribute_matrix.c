#include "read_and_distribute_matrix.h"
#include "utilities.h"


inline int number_of_rows(int id, int ntotal_rows, int p)
{
    return (((id + 1) * ntotal_rows)/p) - ((id * ntotal_rows)/p);
}

inline int owner(int j, int p, int nrows)
{
    return (p * (j + 1) - 1)/nrows;
}

void read_and_distribute_matrix_byrows(char *filename, void ***matrix, void **matrix_storage, MPI_Datatype dtype, int *nrows, int *ncols, int *errval, MPI_Comm comm)
{

    int id;
    int p;
    size_t element_size;
    int mpi_initialized;
    FILE *file;
    int nlocal_rows;
    MPI_Status status;
    const int MSG_TAG = 1;

    MPI_Initialized(&mpi_initialized);
    if (!mpi_initialized) 
    {
        *errval = -1;
        return;
    }

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &id);

    element_size = get_size(dtype);
    if (element_size <= 0) 
    {
        *errval = -1;
        return;
    }

    if (p - 1 == id)
    {
        file = fopen(filename, "r");
        if (NULL == file) {
            *nrows = 0;
            *ncols = 0;
        }
        else
        {
            fread(nrows, sizeof(int), 1, file);
            fread(ncols, sizeof(int), 1, file);
        }
    }

    MPI_Bcast(nrows, 1, MPI_INT, p - 1, comm);

    if (0 == *nrows)
    {
        *errval = -1;
        return;
    }

    MPI_Bcast(ncols, 1, MPI_INT, p - 1, comm);

    nlocal_rows = number_of_rows(id, *nrows, p);

    alloc_matrix(nlocal_rows, *ncols, element_size, matrix_storage, matrix, errval);

    if (SUCCESS != *errval)
    {
        MPI_Abort(comm, *errval);
    }

    if (p - 1 == id)
    {
        int nrows_to_send;
        int num_elements;
        size_t nelements_read;
        int i;

        for (i = 0; i < p - 1; i++)
        {
            nrows_to_send = number_of_rows(i, *nrows, p);
            num_elements = nrows_to_send * (*ncols);
            nelements_read = fread(*matrix_storage, element_size,
                                   num_elements, file);

            if (nelements_read != num_elements)
                MPI_Abort(comm, FILE_READ_ERROR);
            MPI_Send(*matrix_storage, num_elements, dtype,
                     i, MSG_TAG, comm);
        }

        nelements_read = fread(*matrix_storage, element_size,
                               nlocal_rows * (*ncols), file);

        if (nelements_read != nlocal_rows * (*ncols))
            MPI_Abort(comm, FILE_READ_ERROR);

        fclose(file);
    }
    else
    {
        MPI_Recv(*matrix_storage, nlocal_rows * (*ncols),
                 dtype, p - 1, MSG_TAG, comm, &status);
    }
}

void print_matrix(void **matrix, int nrows, int ncols, MPI_Datatype dtype, FILE *stream) {
    int i, j;

    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            if (dtype == MPI_DOUBLE)
                fprintf(stream, "%6.3f ", ((double **)matrix)[i][j]);
            else if (dtype == MPI_FLOAT)
                fprintf(stream, "%6.3f ", ((float **)matrix)[i][j]);
            else if (dtype == MPI_INT)
                fprintf(stream, "%6d ", ((int **)matrix)[i][j]);
        }
        fprintf(stream, "\n");
    }
}

void collect_and_print_matrix(void **matrix, MPI_Datatype dtype, int nrows, int ncols, MPI_Comm comm)
{

    int id;
    int p;
    size_t element_size;
    int nlocal_rows;
    MPI_Status status;
    void **submatrix_buffer;
    void *buffer_storage;
    int max_num_rows;
    int prompt;
    int errval;

    MPI_Comm_rank(comm, &id);
    MPI_Comm_size(comm, &p);

    nlocal_rows = number_of_rows(id, nrows, p);

    if (0 == id)
    {
        int i;

        print_matrix(matrix, nlocal_rows, ncols, dtype, stdout);
        if (p > 1)
        {
            element_size = get_size(dtype);
            max_num_rows = number_of_rows(p - 1, nrows, p);

            alloc_matrix(max_num_rows, ncols, element_size,
                         &buffer_storage,
                         &submatrix_buffer,
                         &errval);
            if (SUCCESS != errval) {
                MPI_Abort(comm, errval);
            }

            for (i = 1; i < p; i++) {
                int num_rows = number_of_rows(i, nrows, p);
                int num_elements = num_rows * ncols;

                MPI_Send(&prompt, 1, MPI_INT, i, PROMPT_MSG, comm);

                MPI_Recv(buffer_storage, num_elements, dtype,
                         i, RESPONSE_MSG, comm, &status);

                print_matrix(submatrix_buffer, num_rows, ncols, dtype, stdout);
            }
            free(submatrix_buffer);
            free(buffer_storage);
        }
        fprintf(stdout, "\n");
    }
    else
    {
        MPI_Recv(&prompt, 1, MPI_INT, 0, PROMPT_MSG, comm, &status);
        MPI_Send(*matrix, nlocal_rows * ncols, dtype, 0, RESPONSE_MSG, comm);
    }
}

