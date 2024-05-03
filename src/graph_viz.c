#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"
#include "mpe.h"
#include "mpe_graphics.h"

struct node_pos{
    int x;
    int y;
};

typedef struct node_pos node_positions;

#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 300
#define GRAPH_SIZE 5
#define NUM_UPPER 10
#define NUM_LOWER 1

int ** generate_random_graph(int rows)
{
    int **graph = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        graph[i] = (int *)malloc(rows * sizeof(int));
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            graph[i][j] = (rand() % (NUM_UPPER - NUM_LOWER) + NUM_LOWER) % 3 == 0; 

            if (graph[i][j] == 1)
            {
                graph[i][j] = (rand() % 100) + 1;
            }
            else
            {
                graph[i][j] == 1000000;
            }
            
            graph[i][i] == 1000000;
        }
        graph[i][i] = 0;
    }

    return graph;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int** graph = generate_random_graph(GRAPH_SIZE);
    node_positions** nodes;
    int numprocs, myid;
    MPI_Comm world;
    MPI_Status status;
    MPE_XGraph handle;

    MPI_Init(&argc,&argv);
    world  = MPI_COMM_WORLD;
    MPI_Comm_size(world,&numprocs);
    MPI_Comm_rank(world,&myid);

    MPE_Open_graphics(&handle, world, NULL, -1, -1, WINDOW_WIDTH, WINDOW_HEIGHT, 1);
    int nodes_num = GRAPH_SIZE;
    if(myid == 0)
    {
        for(int i = 0;i<nodes_num;i++)
        {
            for(int j = 0;j<GRAPH_SIZE;j++)
            {
                printf("%d ", graph[i][j]);
            }
            printf("\n");
        }
        double angle = 360.0/(nodes_num);
        printf("%.2lf\n", angle);
        int circle_x = WINDOW_WIDTH/2;
        int circle_y = WINDOW_HEIGHT/2;
        int radius = 100;

        nodes = (node_positions **)calloc(nodes_num, sizeof(*nodes));

        for(int i = 0;i<nodes_num;i++)
        {
            node_positions * node = (node_positions *)malloc(sizeof(node_positions));
            node->x = circle_x + floor(radius*cos(angle * i * M_PI/180.0));
            node->y = circle_y + floor(radius*sin(angle * i * M_PI/180.0));
            nodes[i] = node;
            char snum[5];
            sprintf(snum, "%d", i+1);

            MPE_Draw_circle(handle,node->x, node->y, radius / 8, MPE_RED);
            MPE_Draw_string(handle, node->x + 2, node->y - 2, MPE_GREEN, snum);
        }
        MPE_Update(handle);

    }

    if(myid == 0)
    {

        for(int i = 0;i<nodes_num;i++)
        {
            for(int j = 0;j<GRAPH_SIZE;j++)
            {
                if(graph[i][j] == 1)
                {
                    printf("Line between [%d](%d, %d) and [%d](%d, %d)\n", i, nodes[i]->x, nodes[i]->y, j, nodes[j]->x, nodes[j]->y);
                    MPE_Draw_line(handle, nodes[i]->x, nodes[i]->y, nodes[j]->x, nodes[j]->y, MPE_BLACK);
                    MPE_Draw_line(handle, nodes[j]->x, nodes[j]->y, nodes[j]->x - 3, nodes[j]->y + 5, MPE_BLACK);
                    MPE_Draw_line(handle, nodes[j]->x, nodes[j]->y, nodes[j]->x + 3, nodes[j]->y + 5, MPE_BLACK);
                }
            }
        }

        MPE_Update(handle);

        for(int i = 0;i<nodes_num;i++)
        {
            free(nodes[i]);
        }
        free(nodes);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPE_Close_graphics(&handle);
    MPI_Finalize();

    for (int k = 0; k < GRAPH_SIZE; k++)
    {
        free(graph[k]);
    }

    free(graph);
    return 0;
}