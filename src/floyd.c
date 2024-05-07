#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include "floyd.h"

int lb(int id, int p, int n)
{  
   return ((id)*(n)/(p));
}

int ub(int id, int p, int n)
{ 
   return (lb((id)+1, p, n)-1);
}

int spn(int id, int p, int n)
{
   return (ub(id, p, n)-lb(id, p, n)+1);
}

int tpn(int j, int p, int n)
{
   return (((p)*((j)+1)-1)/(n));
}

void print_matrix(int **M, int r, int c)
{
   for (int i = 0; i < r; i++) {
      for (int j = 0; j < c; j++) {
         printf("%d ", M[i][j]);
      }
      printf("\n");
   }
}

void assemble_and_print_matrix(void **M, int m, int n)
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

   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   MPI_Comm_size(MPI_COMM_WORLD, &p);
   local_rows = spn(id, p, m);
   if (!id)
   {
      print_matrix((int **)M, local_rows, n);
      if (p > 1)
      {
         max_block_size = spn(p-1, p, m);
         bstorage = malloc(max_block_size * n * sizeof(int));
         b = (void **)malloc(max_block_size * sizeof(int));
         b[0] = bstorage;
         for (i = 1; i < max_block_size; i++)
         {
            b[i] = b[i-1] + n * sizeof(int);
         }
         for (i = 1; i < p; i++)
         {
            MPI_Send(&prompt, 1, MPI_INT, i, PROMPT_MSG, MPI_COMM_WORLD);
            MPI_Recv(bstorage, spn(i, p, m) * n, MPI_INT, i, RESPONSE_MSG, MPI_COMM_WORLD, &status);
            print_matrix((int **)b, spn(i, p, m), n);
         }
         free(b);
         free(bstorage);
      }
      printf("\n");
   } else
   {
      MPI_Recv(&prompt, 1, MPI_INT, 0, PROMPT_MSG, MPI_COMM_WORLD, &status);
      MPI_Send(*M, local_rows * n, MPI_INT, 0, RESPONSE_MSG, MPI_COMM_WORLD);
   }
}

void distribute_matrix(char *s, void ***subs, void **storg, int *m, int *n, int size)
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
   MPI_Comm_size(MPI_COMM_WORLD, &p);
   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   if (id == (p-1))
   {
      int *g = (int *)malloc(size * size * sizeof(int));
      globalArray = (int **)malloc(size * sizeof(int *));
      for (int i = 0; i < size; i++)
      {
         globalArray[i] = &g[i * size];
      }

      FILE * fi = fopen(s, "r");
      for(int i = 0; i < size; i++)
      {
         for(int j = 0; j < size; j++)
         {
            fscanf(fi, "%d", &globalArray[i][j]);
         }
      }
      fclose(fi);
   }

   MPI_Bcast(m, 1, MPI_INT, p-1, MPI_COMM_WORLD);
   MPI_Bcast(n, 1, MPI_INT, p-1, MPI_COMM_WORLD);

   local_rows = spn(id, p, *m);
   *storg = (void *)malloc(local_rows * *n * sizeof(int));
   *subs = (void **)malloc(local_rows * sizeof(void *));

   lptr = (void *)&(*subs[0]);
   rptr = (void *)*storg;

   for (i = 0; i < local_rows; i++)
   {
      *(lptr++)= (void *)rptr;
      rptr += *n * sizeof(int);
   }

   if (id == p-1)
   {
      int total_block_size = 0;
      for (i = 0; i < p-1; i++)
      {
         int block_size = spn(i, p, *m) * *n;
         memcpy(*storg, globalArray[i], block_size * sizeof(int));
         
         MPI_Send(*storg, block_size, MPI_INT, i, DATA_MSG, MPI_COMM_WORLD);

         total_block_size += block_size;
      }

      memcpy(*storg, &(globalArray[0][0]) + total_block_size, local_rows * *n * sizeof(int));
   } else
   {
      MPI_Recv(*storg, local_rows * *n, MPI_INT, p-1, DATA_MSG, MPI_COMM_WORLD, &status);
   }
}

void floyd(int id, int p, int **M, int n)
{   
   int * temp = (int *)malloc(n * sizeof(int));

   for (int k = 0; k < n; k++)
   {
      int source = tpn(k, p, n);
      if (source == id)
      {
         int newidx = k - lb(id, p, n);
         for (int j = 0; j < n; j++)
         {
            temp[j] = M[newidx][j];
         }
      }

      MPI_Bcast(temp, n, MPI_INT, source, MPI_COMM_WORLD);

      for (int i = 0; i < spn(id, p, n); i++)
      {
         for (int j = 0; j < n; j++)
         {
            if (M[i][j] > M[i][k] + temp[j])
            {
               M[i][j] = M[i][k] + temp[j];
            }
         }
      }
   }
   free(temp);
}