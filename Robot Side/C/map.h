#ifndef MAP_H
#define MAP_H

#define HEIGHT 4
#define WIDTH 7

void construct_map(char vmap[HEIGHT][WIDTH]);
void print_map(char vmap[HEIGHT][WIDTH]);
void setObstacle(char vmap[HEIGHT][WIDTH], int x, int y);
void setUnexplored(char vmap[HEIGHT][WIDTH], int x, int y);
void setExplored(char vmap[HEIGHT][WIDTH], int x, int y);
void setRobot(char vmap[HEIGHT][WIDTH], int x, int y);

#endif //MAP_H
