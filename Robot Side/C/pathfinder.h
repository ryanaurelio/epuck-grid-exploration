#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <stdlib.h>

#include "map.h"
#include "types.h"

int check_valid_coordinate(char vmap[HEIGHT][WIDTH], coordinate_node * explored, Coordinate direction);
void move_robot_in_map(char vmap[HEIGHT][WIDTH], int id, Coordinate s, Coordinate t);
void robot_moved_in_map(char vmap[HEIGHT][WIDTH], int id, Coordinate t);
void parse_path(coordinate_node * path, Robot * robot);
coordinate_node * find_path(char vmap[HEIGHT][WIDTH], Coordinate s, Coordinate t);
coordinate_node * get_unexplored_coordinates(char vmap[HEIGHT][WIDTH], int row, int column);
coordinate_node * get_nearest_unexplored(
                char vmap[HEIGHT][WIDTH],coordinate_node * unexploredCoordinates, int row, int column);
        void flood_fill(char vmap[HEIGHT][WIDTH], int row, int column, coordinate_node * reachable);
Coordinate * get_nearest_coordinate(char vmap[HEIGHT][WIDTH], int row, int column);

#endif //PATHFINDER_H
