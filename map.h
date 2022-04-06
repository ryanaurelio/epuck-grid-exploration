#ifndef MAP_H
#define MAP_H

#define HEIGHT 7
#define WIDTH 5

void construct_map(char vmap[HEIGHT][WIDTH]);
//int is_complete(char vmap[HEIGHT][WIDTH]);
void print_map(char vmap[HEIGHT][WIDTH]);
void setObstacle(char vmap[HEIGHT][WIDTH], int x, int y);
void setUnexplored(char vmap[HEIGHT][WIDTH], int x, int y);
int check_unknown(char vmap[HEIGHT][WIDTH], int x, int y);
void setExplored(char vmap[HEIGHT][WIDTH], int x, int y);
//void setRobot(char vmap[HEIGHT][WIDTH], int id, int x, int y);

#endif //MAP_H
