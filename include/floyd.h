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
void print_sub_row_matrix(void **a, int m, int n);
void read_row_striped_matrix(char *s, void ***subs, void **storage, int *m, int *n, int size);
void compute_shortest_paths(int id, int p, int **a, int n);

#endif /* FLOYD_H */
