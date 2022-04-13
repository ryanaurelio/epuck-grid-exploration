#ifndef DYNAMIC_MAP_H
#define DYNAMIC_MAP_H

#include "types.h"

/**
 * Grid linked list that represents a row in map
 */
typedef struct chr_node {
    char symbol;
    Coordinate coordinate;
    struct chr_node * next;
} grid;

/**
 * Get the symbol at a certain coordiante
 * @param head A grid list or row
 * @param x Coordinate x
 * @param y Coordinate y
 * @return The symbol at the given coordinate
 */
char get_symbol(grid * head, int x, int y);

/**
 * Set a symbol at a certain coordinate
 * @param head A grid list or row
 * @param x Coordinate x
 * @param y Coordinate y
 * @param symbol A symbol to be set
 */
void set_symbol(grid * head, int x, int y, char symbol);

/**
 * Generate a new row with starting coordinate (x, y)
 * @param head A pointer for the new row
 * @param x Starting coordinate x
 * @param y Starting coordinate y
 */
void new_row(grid * head, int x, int y);

/**
 * Print a given row
 * @param head A grid list or row to be printed
 */
void print_row(grid * head);

/**
 * Get the length of a given row
 * @param head A grid list or row
 * @return The length of a given row
 */
int row_length(grid * head);

/**
 * Checks whether a given row is empty
 * @param head A grid list or row to be checked
 * @return 1 if the given row is empty, 0 otherwise
 */
int is_row_empty(grid * head);

/**
 * Push a new symbol to the end of the row
 * @param head A grid list or row
 * @param symbol A new symbol to be pushed
 */
void push_front_symbol(grid ** head, char symbol);

/**
 * Push a new symbol to the beginning of the row
 * @param head A grid list or row
 * @param symbol A new symbol to be pushed
 */
void push_back_symbol(grid * head, char symbol);

/**
 * A dynamic 2D array as a map implemented using linked list
 * - '.' : An unexplored grid
 * - 'o' : An explored grid
 * - '0' : Unknown grid outside the map
 * - 'u' : Unknown grid inside the map
 * - 'x' : Obstacle
 */
typedef struct col_node {
    grid * row;
    struct col_node * next;
} dmap;

/**
 * Get the symbol in the map at a certain coordinate
 * @param map A dynamic map
 * @param x Coordinate x
 * @param y Coordinate y
 * @return The symbol in the map at a certain coordinate
 */
char get_symbol_dmap(dmap * map, int x, int y);

/**
 * Set a symbol in the map at a certain coordinate
 * @param map A dynamic map
 * @param x Coordinate x
 * @param y Coordinate y
 * @param symbol A symbol to be set
 */
void set_symbol_dmap(dmap * map, int x, int y, char symbol);

/**
 * Construct a new dynamic map
 * @param map A pointer for the new map
 */
void new_dmap(dmap * map);

/**
 * Prints a given map
 * @param map A map to be printed
 */
void print_dmap(dmap * map);

/**
 * Get the height of a given map
 * @param map A dynamic map
 * @return The height of the map
 */
int get_height(dmap * map);

/**
 * Get the width of a given map
 * @param map A dynamic map
 * @return The width of the map
 */
int get_width(dmap * map);

/**
 * Get the last row of the map
 * @param map A dynamic map
 * @return The last row of the map
 */
grid * map_last(dmap * map);

/**
 * Add a row at the top of the map
 * @param map A dynamic map
 */
void add_row_top(dmap ** map);

/**
 * Add a row at the bottom of the map
 * @param map A dynamic map
 */
void add_row_bottom(dmap * map);

/**
 * Add a column at the left side of the map
 * @param map A dynamic map
 */
void add_column_left(dmap * map);

/**
 * Add a column at the right side of the map
 * @param map A dynamic map
 */
void add_column_right(dmap * map);

/**
 * Expands the map until it reaches the given coordinate
 * @param map A dynamic map to be expanded
 * @param x Target coordinate x
 * @param y Target coordinate y
 */
void expand_dmap(dmap ** map, int x, int y);

/**
 * Flood fill algorithm to checks whether the map is completed
 * @param map A dynamic map to be expanded
 * @param x Starting coordinate x
 * @param y Starting coordinate y
 * @param reachable A list of reachable grids
 * @param is_complete An indicator whether the map is fully explored
 */
void flood_fill(dmap map, int x, int y, coordinate_node * reachable, int * is_complete);

/**
 * Checks whether all the grids in the map have been explored
 * @param map A dynamic map
 * @return 1 if all reachable grids have been explored, 0 otherwise
 */
int is_dmap_complete(dmap map);

/**
 * An optimized algorithm to check whether all the grids in the map have been explored
 * @param map A dynamic map
 * @return 1 if all reachable grids have been explored, 0 otherwise
 */
int is_dmap_complete_optimized(dmap map);

#endif //DYNAMIC_MAP_H
