#include <stdio.h>

#include "map.h"

void construct_map(char vmap[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            vmap[i][j] = (i == 0 || i == HEIGHT-1 || j == 0 || j == WIDTH-1) ? 'x' : '.';
}

int is_complete(char vmap[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            if (vmap[i][j] == '.')
                return 0;
    return 1;
}

void print_map(char vmap[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++)
            printf("%c", vmap[i][j]);
        printf("\n");
    }
}

void setObstacle(char vmap[HEIGHT][WIDTH], int x, int y) {
    vmap[x][y] = 'x';
}

void setUnexplored(char vmap[HEIGHT][WIDTH], int x, int y) {
    vmap[x][y] = '.';
}

void setExplored(char vmap[HEIGHT][WIDTH], int x, int y) {
    vmap[x][y] = 'o';
}

void setRobot(char vmap[HEIGHT][WIDTH], int id, int x, int y) {
    // Max: 9
    char idc = (char) (id - 208);
    vmap[x][y] = idc;
}
