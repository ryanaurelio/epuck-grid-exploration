#ifndef DYNAMIC_MAP_H
#define DYNAMIC_MAP_H

#include "types.h"

// Grid list
typedef struct chr_node {
    char symbol;
    Coordinate coordinate;
    struct chr_node * next;
} grid;

char get_symbol(grid * head, int x, int y);
void set_symbol(grid * head, int x, int y, char symbol);

int row_length(grid * head);
int is_row_empty(grid * head);
void new_row(grid * head, int row, int column);
void print_row(grid * head);
void push_front_symbol(grid ** head, char symbol);
void push_back_symbol(grid * head, char val);

// Map
typedef struct col_node {
    grid * row;
    struct col_node * next;
} dmap;

void new_dmap(dmap * map);
void print_dmap(dmap * map);

char get_symbol_dmap(dmap * map, int x, int y);
void set_symbol_dmap(dmap * map, int x, int y, char symbol);

void expand_dmap(dmap ** map, int x, int y, char symbol);
int get_width(dmap * map);
int get_height(dmap * map);

void add_row_top(dmap ** map);
void add_row_bottom(dmap * map);
void add_column_left(dmap * map);
void add_column_right(dmap * map);


#endif //DYNAMIC_MAP_H
