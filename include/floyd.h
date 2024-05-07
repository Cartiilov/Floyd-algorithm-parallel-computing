#ifndef FLOYD_H
#define FLOYD_H

#define DATA_MSG           0
#define PROMPT_MSG         1
#define RESPONSE_MSG       2

int lb(int id, int p, int n);
int ub(int id, int p, int n);
int spn(int id, int p, int n);
int tpn(int j, int p, int n);

void print_matrix(int **a, int r, int c);
void assemble_and_print_matrix(void **M, int m, int n);
void distribute_matrix(char *s, void ***subs, void **storg, int *m, int *n, int size);
void floyd(int id, int p, int **M, int n);

#endif /* FLOYD_H */
