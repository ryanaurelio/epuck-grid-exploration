#include <stdio.h>
#include <stdlib.h>

#include "dynamic_map.h"

char get_symbol(grid * head, int x, int y) {
    // Construct a coordinate
    Coordinate c = {x, y};

    // Iterate through the row to find the symbol at the given coordinate
    grid * current = head;
    while (current != NULL) {
        if (compare_coordinate(&c, &current->coordinate))
            return current->symbol;
        current = current->next;
    }

    // Returns '0' if the coordinate is not found
    return '0';
}

void set_symbol(grid * head, int x, int y, char symbol) {
    // Construct a coordinate
    Coordinate c = {x, y};

    // Iterate through the row to find right position and set the coordinate there
    grid * current = head;
    while (current != NULL) {
        if (compare_coordinate(&c, &current->coordinate))
            current->symbol = symbol;
        current = current->next;
    }
}

void new_row(grid * head, int x, int y) {
    // Create a new row with symbol 0 and a given coordinate
    head->symbol = 0;
    Coordinate c = {x, y};
    head->coordinate = c;
    head->next = NULL;
}

void print_row(grid * head) {
    // Prints nothing if the row is empty
    if (is_row_empty(head)) {
        printf("None\n");
        return;
    }

    // Print each element in the row
    grid * current = head;
    while (current != NULL) {
        print_coordinate(&current->coordinate);
        printf(" %c  ", current->symbol);
        current = current->next;
    }
    printf("\n");
}

int row_length(grid * head) {
    // If the list is empty returns 0
    if (is_row_empty(head)) return 0;

    // Iterate through the row until it finds the last element while incrementing the length
    int len = 0;
    grid * current = head;
    while (current != NULL) {
        len++;
        current = current->next;
    }
    return len;
}

int is_row_empty(grid * head) {
    // Empty row is defined with element with symbol 0
    return head->symbol == 0;
}

void push_front_symbol(grid ** head, char symbol) {
    if (is_row_empty(*head)){
        // Set the head with the given symbol if the row is still empty
        (*head)->symbol = symbol;
        Coordinate c = {0, 0};
        (*head)->coordinate = c;
    } else {
        // Otherwise, make a new node with the given symbol that points to the current row
        grid *new_node = (grid *) malloc(sizeof(grid));
        new_node->symbol = symbol;
        Coordinate c = {(*head)->coordinate.x-1, (*head)->coordinate.y};
        new_node->coordinate = c;
        new_node->next = *head;
        *head = new_node;
    }
}

void push_back_symbol(grid * head, char symbol) {
    if (is_row_empty(head)) {
        // Set the head with the given symbol if the row is still empty
        head->symbol = symbol;
        Coordinate c = {0, 0};
        head->coordinate = c;
    } else {
        // Otherwise, make a new node with the given symbol and make the last node points to the new node
        grid * current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = (grid *) malloc(sizeof(grid));
        current->next->symbol = symbol;
        Coordinate c = {current->coordinate.x+1, current->coordinate.y};
        current->next->coordinate = c;
        current->next->next = NULL;
    }
}

char get_symbol_dmap(dmap * map, int x, int y) {
    // Iterate through all the rows to find the row with coordinate y and get the symbol at the coordinate using
    // 'get_symbol'
    dmap * current = map;
    while (current != NULL) {
        if (y == current->row->coordinate.y)
            return get_symbol(current->row, x, y);
        current = current->next;
    }

    // Returns '0' if the coordinate is not in map
    return '0';
}

void set_symbol_dmap(dmap * map, int x, int y, char symbol) {
    // Iterate through all the rows to find the row with coordinate y and set the symbol at the coordinate using
    // 'set_symbol'
    dmap * current = map;
    while (current != NULL) {
        if (y == current->row->coordinate.y)
            set_symbol(current->row, x, y, symbol);
        current = current->next;
    }
}

void new_dmap(dmap * map) {
    // Create a 1x1 map with symbol u
    grid * row = (grid *) malloc(sizeof(grid));
    row->symbol = 'u';
    Coordinate c = {0, 0};
    row->coordinate = c;
    row->next = NULL;
    map->row = row;
    map->next = NULL;
}

void print_dmap(dmap * map) {
    // Iterate through the rows and print them
    dmap * current = map;
    while (current != NULL) {
        print_row(current->row);
        current = current->next;
    }
}

int get_height(dmap * map) {
    // Iterate through all the rows while incrementing the height
    int height = 0;
    dmap * current = map;
    while (current != NULL) {
        height++;
        current = current->next;
    }
    return height;
}

int get_width(dmap * map) {
    // Returns the length of a row in the map
    return row_length(map->row);
}

grid * map_last(dmap * map) {
    // Iterate through all the rows to get the last row
    dmap * current = map;
    while (current->next != NULL)
        current = current->next;
    return current->row;
}

void add_row_top(dmap ** map) {
    // Generate a row of 'u's with the proper coordinate
    grid * row = (grid *) malloc(sizeof(grid));
    row->symbol = 'u';
    Coordinate c = {(*map)->row->coordinate.x, (*map)->row->coordinate.y+1};
    row->coordinate = c;
    row->next = NULL;
    for (int i = 0; i < row_length((*map)->row)-1; i++)
        push_back_symbol(row, 'u');

    // Change the pointer to the new row
    dmap * new_node = (dmap *) malloc(sizeof(dmap));
    new_node->row = row;
    new_node->next = *map;
    *map = new_node;
}

void add_row_bottom(dmap * map) {
    // Generate a row of 'u's with the proper coordinate
    grid * row = (grid *) malloc(sizeof(grid));
    row->symbol = 'u';
    Coordinate c = {map->row->coordinate.x, map_last(map)->coordinate.y-1};
    row->coordinate = c;
    row->next = NULL;
    for (int i = 0; i < row_length(map->row)-1; i++)
        push_back_symbol(row, 'u');

    // Make the last row points to the new row
    dmap * current = map;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = (dmap *) malloc(sizeof(dmap));
    current->next->row = row;
    current->next->next = NULL;
}

void add_column_left(dmap * map) {
    // Push symbol 'u' into the end of every row
    dmap * current = map;
    while (current != NULL) {
        push_front_symbol(&current->row, 'u');
        current = current->next;
    }
}

void add_column_right(dmap * map) {
    // Push symbol 'u' into the beginning of every row
    dmap * current = map;
    while (current != NULL) {
        push_back_symbol(current->row, 'u');
        current = current->next;
    }
}

void expand_dmap(dmap ** map, int x, int y) {
    // Bottom right coordinate
    grid * last = map_last(*map);
    while (last->next != NULL) {
        last = last->next;
    }

    // Expand map
    // Coordinate above the map
    int top_diff = y - (*map)->row->coordinate.y;
    for (int i = 0; i < top_diff; i++)
        add_row_top(map);

    // Coordinate on the left side of the map
    int left_diff = (*map)->row->coordinate.x - x;
    for (int i = 0; i < left_diff; i++)
        add_column_left(*map);

    // Coordinate on the right side of the map
    int right_diff = x - last->coordinate.x;
    for (int i = 0; i < right_diff; i++)
        add_column_right(*map);

    // Coordinate below the map
    int bot_diff = last->coordinate.y - y;
    for (int i = 0; i < bot_diff; i++)
        add_row_bottom(*map);
}

void flood_fill(dmap map, int x, int y, coordinate_node * reachable, int * is_complete) {
    // Stop the recursion if the map is not completed
    if (!is_complete) return;

    // Map is not complete if the algorithm finds an unexplored grid
    char symbol = get_symbol_dmap(&map, x, y);
    if (symbol == 'u' || symbol == '0' || symbol == '.') {
        *is_complete = 0;
        return;
    }

    // Prevent the recursion to check the current path further if it finds an obstacle or if an area is already covered
    // by the algorithm
    Coordinate c = {x, y};
    if (symbol == 'x' || coordinate_list_contains(reachable, &c)) return;

    // Push the coordinate into reachable list
    push_coordinate_list(reachable, c);

    // Cover the next grid for each direction
    flood_fill(map, x, y-1, reachable, is_complete);     // South
    flood_fill(map, x, y+1, reachable, is_complete);     // North
    flood_fill(map, x-1, y, reachable, is_complete);     // West
    flood_fill(map, x+1, y, reachable, is_complete);     // East
}

int is_dmap_complete(dmap map) {
    // Checks whether the area is surrounded by obstacle using the flood fill algorithm
    coordinate_node * reachable;
    reachable = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(reachable);
    int * is_complete = (int *) malloc(sizeof(int));
    * is_complete = 1;
    flood_fill(map, 0, 0, reachable, is_complete);

    // Clean up memory
    int retval = * is_complete;
    free(reachable);
    free(is_complete);
    return retval;
}

int is_dmap_complete_optimized(dmap map) {
    dmap * current_row = &map;
    while (current_row != NULL) {
        grid * current_c = current_row->row;
        while (current_c != NULL) {
            char symbol = current_c->symbol;
            // Checks whether there is still an unexplored grid
            if (symbol == '.') return 0;
            // Checks whether there is still unknown grids near the robots
            if (symbol != 'o' && symbol != '0' && symbol != 'x' && symbol != 'u'){
                // East
                char east = get_symbol_dmap(&map, current_c->coordinate.x+1, current_c->coordinate.y);
                if (east == 'u' || east == '0') return 0;
                // South
                char south = get_symbol_dmap(&map, current_c->coordinate.x, current_c->coordinate.y-1);
                if (south == 'u' || south == '0') return 0;
                // West
                char west = get_symbol_dmap(&map, current_c->coordinate.x-1, current_c->coordinate.y);
                if (west == 'u' || west == '0') return 0;
                // North
                char north = get_symbol_dmap(&map, current_c->coordinate.x, current_c->coordinate.y+1);
                if (north == 'u' || north == '0') return 0;
            }
            current_c = current_c->next;
        }
        current_row = current_row->next;
    }
    // Return 1 if the map is fully explored
    return 1;
}
