#include <upc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 10000

int lb(int id, int p, int n)
{
    return ((id) * (n) / (p));
}

int ub(int id, int p, int n)
{
    return (lb((id) + 1, p, n) - 1);
}

int spn(int id, int p, int n)
{
    return (ub(id, p, n) - lb(id, p, n) + 1);
}

int tpn(int j, int p, int n)
{
    return (((p) * ((j) + 1) - 1) / (n));
}

void print_matrix(shared int *M, int r, int c)
{
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < c; j++)
        {
            printf("%6d ", M[i * c + j]);
        }
        printf("\n");
    }
}

void assemble_and_print_matrix(shared int *M, int m, int n)
{
    if (MYTHREAD == 0)
    {
        printf("\n==========\n");
        print_matrix(M, m, n);
        printf("==========\n");
    }
}

void distribute_matrix(const char *filename, shared int *M, int m, int n)
{
    if (MYTHREAD == THREADS - 1)
    {
        int **globalArray = (int **)malloc(m * sizeof(int *));
        int *global1DArray = (int *)malloc(m * n * sizeof(int));
        for (int i = 0; i < m; i++)
        {
            globalArray[i] = (int *)malloc(n * sizeof(int));
        }

        FILE *fi = fopen(filename, "r");
        if (!fi)
        {
            fprintf(stderr, "Error opening file %s\n", filename);
            upc_global_exit(1);
        }

        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                fscanf(fi, "%d", &globalArray[i][j]);
                global1DArray[i * n + j] = globalArray[i][j];
            }
        }
        fclose(fi);

        for (int i = 0; i < THREADS - 1; i++)
        {
            int block_size = spn(i, THREADS, m) * n;
            int start_idx = lb(i, THREADS, m) * n;

            for (int j = 0; j < block_size; j++)
            {
                upc_memput(&M[start_idx + j], &global1DArray[start_idx + j], sizeof(int));
            }
        }
        int last_block_size = spn(THREADS - 1, THREADS, m) * n;
        int start_idx = lb(THREADS - 1, THREADS, m) * n;
        for (int j = 0; j < last_block_size; j++)
        {
            upc_memput(&M[start_idx + j], &global1DArray[start_idx + j], sizeof(int));
        }

        for (int i = 0; i < m; i++)
        {
            free(globalArray[i]);
        }
        free(globalArray);
    }
    upc_barrier;
}

void floyd(int id, int p, shared int *M, int n)
{
    for (int k = 0; k < n; k++)
    {
        int start = lb(id, p, n);
        int end = ub(id, p, n);

        for (int i = start; i <= end; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (M[i * n + j] > M[i * n + k] + M[k * n + j])
                {
                    M[i * n + j] = M[i * n + k] + M[k * n + j];
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int m, n, size, show_messages;

    if (argc < 4)
    {
        printf("Insufficient number of command-line arguments - terminating\n");
        return -1;
    }

    size = atoi(argv[1]);
    show_messages = atoi(argv[3]);

    m = size;
    n = size;

    if (MYTHREAD == 0 && show_messages != 0)
    {
        if (THREADS > size)
        {
            printf("Threads is %d", THREADS);
            printf("Number of threads greater than number of matrix rows - terminating\n");
            return -1;
        }

        if (MYTHREAD == 0)
        {
            printf("Matrix size: %d, Using threads: %d\n", size, THREADS);
        }

        printf("Matrix size: %d, Using threads: %d\n", size, THREADS);
    }

    shared int *M = (shared int *)upc_all_alloc(m * n, sizeof(int));

    distribute_matrix(argv[2], M, m, n);

    upc_barrier;

    floyd(MYTHREAD, THREADS, M, n);
    upc_barrier;

    assemble_and_print_matrix(M, m, n);

    return 0;
}