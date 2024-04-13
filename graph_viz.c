#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

int main(int argc, char *argv[]){

    int graph[][5] = {{0,1,1,0,0},{1,0,0,1,0},{1,0,0,0,0},{0,1,0,0,0}, {1,1,1,0,0}};
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
    int nodes_num = sizeof(graph[0]) / sizeof(int);
    if(myid == 0){
        for(int i = 0;i<nodes_num;i++){
            for(int j = 0;j<sizeof(graph[1])/sizeof(int);j++){
                printf("%d ", graph[i][j]);
            }
            printf("\n");
        }
        double angle = 360.0/(nodes_num);
        printf("%.2lf\n", angle);
        int circle_x = WINDOW_WIDTH/2;
        int circle_y = WINDOW_HEIGHT/2;
        int radius = 50;

        nodes = (node_positions **)calloc(nodes_num, sizeof(*nodes));

        for(int i = 0;i<nodes_num;i++){
            node_positions * node = (node_positions *)malloc(sizeof(node_positions));
            node->x = circle_x + floor(radius*cos(angle * i * M_PI/180.0));
            node->y = circle_y + floor(radius*sin(angle * i * M_PI/180.0));
            nodes[i] = node;
            char snum[5];
            sprintf(snum, "%d", i+1);

            MPE_Draw_circle(handle,node->x, node->y, radius / 4, MPE_RED);
            MPE_Draw_string(handle, node->x, node->y, MPE_GREEN, snum);
        }
        MPE_Update(handle);

    }

    if(myid == 0){

        for(int i = 0;i<nodes_num;i++){
            for(int j = 0;j<sizeof(graph[j])/(sizeof(int));j++){
                if(graph[i][j] == 1){
                    printf("Line between [%d](%d, %d) and [%d](%d, %d)\n", i, nodes[i]->x, nodes[i]->y, j, nodes[j]->x, nodes[j]->y);
                    MPE_Draw_line(handle, nodes[i]->x, nodes[i]->y, nodes[j]->x, nodes[j]->y, MPE_BLACK);
                }
            }
        }

        MPE_Update(handle);

        for(int i = 0;i<nodes_num;i++){
            free(nodes[i]);
        }
        free(nodes);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPE_Close_graphics(&handle);
    MPI_Finalize();
    return 0;
}