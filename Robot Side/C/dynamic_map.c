#include <stdio.h>
#include <stdlib.h>

#include "dynamic_map.h"

// Character list
char get_symbol(grid * head, int x, int y) {
    grid * current = head;
    Coordinate c = {x, y};

    while (current != NULL) {
        if (compare_coordinate(&c, &current->coordinate))
            return current->symbol;
        current = current->next;
    }

    return '0';
}

void set_symbol(grid * head, int x, int y, char symbol) {
    grid * current = head;
    Coordinate c = {x, y};

    while (current != NULL) {
        if (compare_coordinate(&c, &current->coordinate))
            current->symbol = symbol;
        current = current->next;
    }
}

int row_length(grid * head) {
    if (is_row_empty(head)) return 0;

    grid * current = head;
    int len = 0;

    while (current != NULL) {
        len++;
        current = current->next;
    }
    return len;
}

int is_row_empty(grid * head) {
    return head->symbol == 0;
}

void new_row(grid * head, int row, int column) {
    head->symbol = 0;
    Coordinate c = {row, column};
    head->coordinate = c;
    head->next = NULL;
}

void print_row(grid * head) {
    if (is_row_empty(head)) {
        printf("None\n");
        return;
    }
    grid * current = head;

    while (current != NULL) {
        print_coordinate(&current->coordinate);
        printf(" %c  ", current->symbol);
        current = current->next;
    }
    printf("\n");
}

void push_front_symbol(grid ** head, char symbol) {
    if (is_row_empty(*head)){
        (*head)->symbol = symbol;
        Coordinate c = {0, 0};
        (*head)->coordinate = c;
    } else {
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
        head->symbol = symbol;
        Coordinate c = {0, 0};
        head->coordinate = c;
    } else {
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

// Map
void new_dmap(dmap * map) {
    grid * row = (grid *) malloc(sizeof(grid));
    row->symbol = 'u';
    Coordinate c = {0, 0};
    row->coordinate = c;
    row->next = NULL;

    map->row = row;
    map->next = NULL;
}

void print_dmap(dmap * map) {
    dmap * current = map;

    while (current != NULL) {
        print_row(current->row);
        current = current->next;
    }
}

char get_symbol_dmap(dmap * map, int x, int y) {
    dmap * current = map;

    while (current != NULL) {
        if (y == current->row->coordinate.y)
            return get_symbol(current->row, x, y);
        current = current->next;
    }

    return '0';
}

void set_symbol_dmap(dmap * map, int x, int y, char symbol) {
    dmap * current = map;

    while (current != NULL) {
        if (y == current->row->coordinate.y)
            set_symbol(current->row, x, y, symbol);
        current = current->next;
    }
}

int get_width(dmap * map) {
    return row_length(map->row);
}

int get_height(dmap * map) {
    int height = 0;
    dmap * current = map;
    while (current != NULL) {
        height++;
        current = current->next;
    }
    return height;
}

grid * map_last(dmap * map) {
    dmap * current = map;
    while (current->next != NULL)
        current = current->next;
    return current->row;
}

void add_row_top(dmap ** map) {
    grid * row = (grid *) malloc(sizeof(grid));
    row->symbol = 'u';
    Coordinate c = {(*map)->row->coordinate.x, (*map)->row->coordinate.y+1};
    row->coordinate = c;
    for (int i = 0; i < row_length((*map)->row)-1; i++)
        push_back_symbol(row, 'u');

    dmap * new_node = (dmap *) malloc(sizeof(dmap));;
    new_node->row = row;
    new_node->next = *map;
    *map = new_node;
}

void add_row_bottom(dmap * map) {
    grid * row = (grid *) malloc(sizeof(grid));
    row->symbol = 'u';
    Coordinate c = {map->row->coordinate.x, map_last(map)->coordinate.y-1};
    row->coordinate = c;
    for (int i = 0; i < row_length(map->row)-1; i++)
        push_back_symbol(row, 'u');

    dmap * current = map;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = (dmap *) malloc(sizeof(dmap));
    current->next->row = row;
    current->next->next = NULL;
}

void add_column_left(dmap * map) {
    dmap * current = map;
    while (current != NULL) {
        push_front_symbol(&current->row, 'u');
        current = current->next;
    }
}

void add_column_right(dmap * map) {
    dmap * current = map;
    while (current != NULL) {
        push_back_symbol(current->row, 'u');
        current = current->next;
    }
}
