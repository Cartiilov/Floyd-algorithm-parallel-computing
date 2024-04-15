#include <utilities.h>

#define SUCCESS 0
#define OUT_OF_MEM_ERR 1


void alloc_matrix(int nrows, int ncols, size_t element_size, void **matrix_storage, void ***matrix, int **errvalue)
{
    int i;
    void* ptr_to_row_in_storage; 
    void** matrix_row_start;
    *matrix_storage = malloc ( nrows * ncols * element_size ) ;
    if ( NULL == *matrix_storage ) {
        *errvalue = OUT_OF_MEM_ERR;
        return ;
    }

    *matrix = malloc(nrows * sizeof (void*));
    if (NULL == *matrix) {
        *errvalue = OUT_OF_MEM_ERR;
        return;
    }

    matrix_row_start = (void*) &(*matrix[0]);

    ptr_to_row_in_storage = (void *)*matrix_storage;

    for (i = 0; i < nrows ; i++) {
        *matrix_row_start = (void*)ptr_to_row_in_storage;

        matrix_row_start++;
        ptr_to_row_in_storage += ncols * element_size; 
    }
    **errvalue = SUCCESS;
}




