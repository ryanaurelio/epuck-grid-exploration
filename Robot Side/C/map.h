#ifndef MAP_H
#define MAP_H

/*
 * The map has the following symbols:
 * - '.' : An unexplored grid
 * - 'o' : An explored grid
 * - 'x' : Obstacle
 */

// Modify this size
#define HEIGHT 7
#define WIDTH 5

/**
 * Construct a new map
 * @param vmap A pointer to a map
 */
void construct_map(char vmap[HEIGHT][WIDTH]);

/**
 * Checks whether the map has been fully explored
 * @param vmap A pointer to a map
 * @return 1 if the map is completed, 0 otherwise
 */
int is_complete(char vmap[HEIGHT][WIDTH]);

/**
 * Print a given map
 * @param vmap A pointer to a map
 */
void print_map(char vmap[HEIGHT][WIDTH]);

/**
 * Set an unexplored symbol '.' at a certain coordinate
 * @param vmap A pointer to a map
 * @param x Coordinate x
 * @param y Coordinate y
 */
void setUnexplored(char vmap[HEIGHT][WIDTH], int x, int y);

/**
 * Set an explored symbol 'o' at a certain coordinate
 * @param vmap A pointer to a map
 * @param x Coordinate x
 * @param y Coordinate y
 */
void setExplored(char vmap[HEIGHT][WIDTH], int x, int y);

/**
 * Set an obstacle symbol 'x' at a certain coordinate
 * @param vmap A pointer to a map
 * @param x Coordinate x
 * @param y Coordinate y
 */
void setObstacle(char vmap[HEIGHT][WIDTH], int x, int y);

/**
 * Set a robot symbol (indicated with the ID of the robot) at a certain coordinate
 * @param vmap A pointer to a map
 * @param id ID of the robot
 * @param x Coordinate x
 * @param y Coordinate y
 */
void setRobot(char vmap[HEIGHT][WIDTH], int id, int x, int y);

#endif //MAP_H
