#include <stdio.h>

#include "map.h"

void construct_map(char vmap[HEIGHT][WIDTH]) {
    // Set a border for the map with 'x' and '.' everywhere else
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            vmap[i][j] = (i == 0 || i == HEIGHT-1 || j == 0 || j == WIDTH-1) ? 'x' : '.';
}

int is_complete(char vmap[HEIGHT][WIDTH]) {
    // Returns 0 if unexplored symbol '.' is found
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            if (vmap[i][j] == '.')
                return 0;
    return 1;
}

void print_map(char vmap[HEIGHT][WIDTH]) {
    // Iterate through all the symbols and print them
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++)
            printf("%c", vmap[i][j]);
        printf("\n");
    }
}

void setUnexplored(char vmap[HEIGHT][WIDTH], int x, int y) {
    // Set the symbol with '.' at the given location
    vmap[x][y] = '.';
}

void setExplored(char vmap[HEIGHT][WIDTH], int x, int y) {
    // Set the symbol with 'o' at the given location
    vmap[x][y] = 'o';
}

void setObstacle(char vmap[HEIGHT][WIDTH], int x, int y) {
    // Set the symbol with 'x' at the given location
    vmap[x][y] = 'x';
}

void setRobot(char vmap[HEIGHT][WIDTH], int id, int x, int y) {
    // Set the symbol with the ID of the robot at the given location
    char idc = (char) (id - 208);
    vmap[x][y] = idc;
}
