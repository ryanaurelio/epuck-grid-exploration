#ifndef DMAP_PATHFINDER_H
#define DMAP_PATHFINDER_H

#include "dynamic_map.h"

/**
 * A depth first search algorithm for finding an unexplored grid
 * @param map A map to be explored
 * @param x Starting coordinate x
 * @param y Starting coordinate y
 * @param explored Explored grids
 * @param unexplored An unexplored coordinate to be found
 * @param is_found An indicator when the unexplored coordinate has been found
 */
void dfs_unexplored(dmap map, int x, int y, coordinate_node * explored, Coordinate * unexplored, int * is_found);

/**
 * Check whether a coordinate is valid:
 * - The coordinate hasn't been explored
 * - The robot can stand on the coordinate ('.'/'o')
 * @param map A map to be checked
 * @param explored A list of grid that have been explored
 * @param direction Next direction too be explored
 * @return 1 if the coordinate in the given direction is valid, otherwise 0
 */
int check_valid_coordinate_dmap(dmap map, coordinate_node * explored, Coordinate direction);

/**
 * Get a path (sequence of coordinates) to the nearest unexplored coordinate
 * @param map A map to be explored
 * @param unexploredCoordinates List of unexplored coordinates
 * @param x Starting coordinate x
 * @param y Starting coordinate y
 * @return A path to the nearest unexplored coordinate
 */
coordinate_node * get_nearest_unexplored_dmap(dmap map, coordinate_node * unexploredCoordinates, int x, int y);

/**
 * Move the physical robot in map
 * @param robot The robot to be moved
 * @param seq The move sequence encoded in 'WASD'
 */
void move_path(Robot * robot, grid * seq);

/**
 * Parse the path into move sequence to be passed to 'move_path'
 * @param path A list of coordinates of the movement sequence
 * @param robot A robot that is going to explore
 */
void parse_path_dmap(coordinate_node * path, Robot * robot);

/**
 * Mark the path in the map that is going to be explored and move the physical robot
 * @param map A map to be explored
 * @param id The ID of the robot that is going to be moved
 * @param s Starting coordinate
 * @param t Target coordinate
 */
void move_robot_in_dmap(dmap map, int id, Coordinate s, Coordinate t);

/**
 * Modify the explored path with explored symbol 'o'
 * @param map A map to be explored
 * @param id The ID of the robot that finished the exploration
 * @param t The current location od the robot
 */
void robot_moved_in_dmap(dmap map, int id, Coordinate target);

/**
 * Find path in map within 2 grids
 * @param map A map to be explored
 * @param s Starting coordinate
 * @param t Target coordinate
 * @return A list of coordinates of the movement sequence
 */
coordinate_node * find_path_dmap(dmap map, Coordinate s, Coordinate t);

/**
 * Get the nearest unexplored coordinate
 * @param map A map to be explored
 * @param x Starting coordinate x
 * @param y Starting coordinate y
 * @return A nearest unexplored coordinate
 */
Coordinate * get_nearest_coordinate_dmap(dmap map, int x, int y);

#endif //DMAP_PATHFINDER_H
