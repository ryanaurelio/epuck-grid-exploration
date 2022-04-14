#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <stdlib.h>

#include "map.h"
#include "types.h"
#include "dynamic_map.h"


/**
 * A flood fill algorithm to find all the reachable grids
 * @param vmap A map to be checked
 * @param row Starting row
 * @param column Starting column
 * @param reachable A list for all the reachable grids
 */
void flood_fill(char vmap[HEIGHT][WIDTH], int row, int column, coordinate_node * reachable);

/**
 * Check whether a coordinate is valid:
 * - The coordinate hasn't been explored
 * - The robot can stand on the coordinate ('.'/'o')
 * @param vmap A map to be checked
 * @param explored A list of grid that have been explored
 * @param direction Next direction too be explored
 * @return 1 if the coordinate in the given direction is valid, otherwise 0
 */
int check_valid_coordinate(char vmap[HEIGHT][WIDTH], coordinate_node * explored, Coordinate direction);

/**
 * Get all the reachable unexplored grids
 * @param vmap A map to be checked
 * @param row Starting row
 * @param column Starting column
 * @return A list of reachable unexplored grids
 */
coordinate_node * get_unexplored_coordinates(char vmap[HEIGHT][WIDTH], int row, int column);

/**
 * Get a path (sequence of coordinates) to the nearest unexplored coordinate
 * @param vmap A map to be explored
 * @param unexploredCoordinates List of unexplored coordinates
 * @param row Starting row
 * @param column Starting column
 * @return A path to the nearest unexplored coordinate
 */
coordinate_node * get_nearest_unexplored(char vmap[HEIGHT][WIDTH],coordinate_node * unexploredCoordinates, int row,
                                         int column);
/**
 * Parse the path into move sequence to be passed to 'move_path'
 * @param path A list of coordinates of the movement sequence
 * @param robot A robot that is going to explore
 */
void parse_path(coordinate_node * path, Robot * robot);

/**
 * Mark the path in the map that is going to be explored and move the physical robot
 * @param vmap A map to be explored
 * @param id The ID of the robot that is going to be moved
 * @param s Starting coordinate
 * @param t Target coordinate
 */
void move_robot_in_map(char vmap[HEIGHT][WIDTH], int id, Coordinate s, Coordinate t);

/**
 * Modify the explored path with explored symbol 'o'
 * @param vmap A map to be explored
 * @param id The ID of the robot that finished the exploration
 * @param t The current location od the robot
 */
void robot_moved_in_map(char vmap[HEIGHT][WIDTH], int id, Coordinate t);

/**
 * Find path in map within 2 grids
 * @param vmap A map to be explored
 * @param s Starting coordinate
 * @param t Target coordinate
 * @return A list of coordinates of the movement sequence
 */
coordinate_node * find_path(char vmap[HEIGHT][WIDTH], Coordinate s, Coordinate t);

/**
 * Get the nearest unexplored coordinate
 * @param vmap A map to be explored
 * @param x Starting coordinate x
 * @param y Starting coordinate y
 * @return A nearest unexplored coordinate
 */
Coordinate * get_nearest_coordinate(char vmap[HEIGHT][WIDTH], int row, int column);

#endif //PATHFINDER_H
