#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <stdlib.h>

#include "dynamic_map.h"
#include "map.h"
#include "types.h"

void move_robot_in_dmap(dmap map, int id, Coordinate s, Coordinate t);
void robot_moved_in_dmap(dmap map, int id, Coordinate t);
void parse_path_dmap(coordinate_node * path, Robot * robot);
coordinate_node * find_path_dmap(dmap map, Coordinate s, Coordinate t);
coordinate_node * get_unexplored_coordinates_dmap(dmap map, int x, int y);
coordinate_node * get_nearest_unexplored_dmap(dmap map, coordinate_node * unexploredCoordinates, int x, int y);
Coordinate * get_nearest_coordinate_dmap(dmap map, int x, int y);

#endif //PATHFINDER_H