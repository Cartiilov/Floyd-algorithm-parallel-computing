#ifndef UTILITIES
#define UTILITIES

#include <stdlib.h>

#define SUCCESS 0
#define OUT_OF_MEM_ERR 1

void alloc_matrix(int nrows, int ncols, size_t element_size, void **matrix_storage, void ***matrix, int *errvalue);

#endif /* MATRIX_ALLOC_H */
