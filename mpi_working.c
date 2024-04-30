#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define DATA_MSG           0
#define PROMPT_MSG         1
#define RESPONSE_MSG       2

int lb(int id, int p, int n){  
   return ((id)*(n)/(p));
}
int ub(int id,int p,int n){ 
   return (lb((id)+1,p,n)-1);
}
int spn(int id, int p, int n) {
   return (ub(id,p,n)-lb(id,p,n)+1);
}
int tpn(int j,int p,int n) {
   return (((p)*((j)+1)-1)/(n));
}

void print_matrix (int **a, int r, int c)    
{
   for (int i = 0; i < r; i++) {
      for (int j = 0; j < c; j++) {
         printf ("%d ", a[i][j]);
      }
      printf("\n");
   }
}

void print_sub_row_matrix (void **a, int m, int n)      
{
   MPI_Status  status;          
   void       *bstorage;       
   void      **b;               
   int         i;
   int         id;              
   int         local_rows;      
   int         max_block_size;  
   int         prompt;          
   int         p;               

   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   MPI_Comm_size (MPI_COMM_WORLD, &p);
   local_rows = spn(id,p,m);
   if (!id) {
      print_matrix ((int**)a, local_rows, n);
      if (p > 1) {
         max_block_size = spn(p-1,p,m);
         bstorage = malloc (
            max_block_size * n * sizeof(int));
         b = (void **) malloc (
            max_block_size * sizeof(int));
         b[0] = bstorage;
         for (i = 1; i < max_block_size; i++) {
            b[i] = b[i-1] + n * sizeof(int);
         }
         for (i = 1; i < p; i++) {
            MPI_Send (&prompt, 1, MPI_INT, i, PROMPT_MSG,
               MPI_COMM_WORLD);
            MPI_Recv (bstorage, spn(i,p,m)*n, MPI_INT,
               i, RESPONSE_MSG, MPI_COMM_WORLD, &status);
            print_matrix ((int**)b, spn(i,p,m), n);
         }
         free (b);
         free (bstorage);
      }
      printf ("\n");
   } else {
      MPI_Recv (&prompt, 1, MPI_INT, 0, PROMPT_MSG,
         MPI_COMM_WORLD, &status);
      MPI_Send (*a, local_rows * n, MPI_INT, 0, RESPONSE_MSG,
         MPI_COMM_WORLD);
   }
}

void read_row_striped_matrix (char *s, void ***subs, void **storage, int *m,        
   int *n, int size)     
{
   int          i;
   int          id;           
   int          local_rows;   
   void       **lptr;         
   int          p;            
   void        *rptr;         
   MPI_Status   status;       
   int          x;            
   int** globalArray = NULL; 

   *n = size;
   *m = size;
   MPI_Comm_size (MPI_COMM_WORLD, &p);
   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   if (id == (p-1)) {
      int *g = (int *)malloc(size * size * sizeof(int));
      globalArray = (int **)malloc(size * sizeof(int *));
      for (int i = 0; i < size; i++)
      {
         globalArray[i] = &g[i * size];
      }

      FILE * fi = fopen(s, "r");
      for(int i = 0;i<size;i++){
         for(int j = 0;j<size;j++){
               fscanf(fi, "%d", &globalArray[i][j]);
         }
      }
      fclose(fi);
   
   }

   MPI_Bcast (m, 1, MPI_INT, p-1, MPI_COMM_WORLD);

   MPI_Bcast (n, 1, MPI_INT, p-1, MPI_COMM_WORLD);

   local_rows = spn(id,p,*m);
   *storage = (void *) malloc (
       local_rows * *n * sizeof(int));
   *subs = (void **) malloc (local_rows * sizeof(void *));

   lptr = (void *) &(*subs[0]);
   rptr = (void *) *storage;
   for (i = 0; i < local_rows; i++) {
      *(lptr++)= (void *) rptr;
      rptr += *n * sizeof(int);
   }

   if (id == p-1) {
      int total_block_size = 0;
      for (i = 0; i < p-1; i++) {
         int block_size = spn(i,p,*m) * *n;
         memcpy(*storage, globalArray[i], block_size * sizeof(int));
         
         MPI_Send (*storage, block_size, MPI_INT, i, DATA_MSG, MPI_COMM_WORLD);

         total_block_size += block_size;
      }

      memcpy(*storage, &(globalArray[0][0]) + total_block_size, local_rows * *n * sizeof(int));
   } else{
      MPI_Recv (*storage, local_rows * *n, MPI_INT, p-1, DATA_MSG, MPI_COMM_WORLD, &status);
   }
}

void compute_shortest_paths (int id, int p, int **a, int n)
{   
   int * tmp = (int *) malloc (n * sizeof(int));
   int pred[n][n];

   for (int i = 0; i < n; i++)
   {
      for (int j = 0; j < n; j++)
      {
         pred[i][j] = i;
      }
      pred[i][i] = i;
   }


   for (int k = 0; k < n; k++) {
      int root = tpn(k,p,n);
      if (root == id) {
         int offset = k - lb(id,p,n);
         for (int j = 0; j < n; j++){
            tmp[j] = a[offset][j];
         }
      }

      MPI_Bcast (tmp, n, MPI_INT, root, MPI_COMM_WORLD);

      for (int i = 0; i < spn(id,p,n); i++)
         for (int j = 0; j < n; j++){
            if(a[i][j] > a[i][k] + tmp[j]){
               a[i][j] = a[i][k]+tmp[j];
               pred[i][j] = pred[k][j];
            }
         }
   }
   free(tmp);


      int source, target, k;
      int path[n];
      for (int z = 0; z < n; ++z)
      {
         path[z] = 0;
      }

      for (int i = 0; i < n; ++i)
      {
         for (int j =0; j < i; ++j)
         {
            source = i;
            target = j;

            k = 0;
            while (source != target)
            {
               target = pred[source][target];
               path[k] = target;
               k++;
            }
            printf("%d -> %d: ", source, target);
            printf("%d ", target);
            for (int z = n - 1; z >= 0; --z)
            {
               if (path[z] != 0)
                  printf("%d ", path[z]);
                  path[z] = 0;
            }
            printf("\n");
         }
      }
      printf("\n");
}

int main (int argc, char *argv[]) {
   int** a;         
   int*  storage;   
   int     id;        
   int     m;         
   int     n;         
   int     p;         
   int size;


   MPI_Init (&argc, &argv);
   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   MPI_Comm_size (MPI_COMM_WORLD, &p);

   size = atoi(argv[1]);
   if(p > size){
      printf("Number of nodes greater than number of matrix rows - terminating");
      MPI_Abort(MPI_COMM_WORLD, -1);
      MPI_Finalize();
      return -1;
   }else if(argc < 3){
      printf("Insufficient number of command-line arguments - terminating");
      MPI_Abort(MPI_COMM_WORLD, -1);
      MPI_Finalize();
      return -1;
   }

   read_row_striped_matrix (argv[2], (void *) &a,
      (void *) &storage, &m, &n, size);

   MPI_Barrier (MPI_COMM_WORLD);
   compute_shortest_paths (id, p, (int **) a, n);

   print_sub_row_matrix ((void **) a, m, n);

   MPI_Finalize();
   return 0;
}