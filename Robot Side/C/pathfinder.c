#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "types.h"

void flood_fill(char vmap[HEIGHT][WIDTH], int row, int column, coordinate_node * reachable) {
    char symbol = vmap[row][column];
    Coordinate c = {row, column};
    if (symbol == 'x' || coordinate_list_contains(reachable, &c))
        return;

    push_coordinate_list(reachable, c);
    flood_fill(vmap, row+1, column, reachable);     // South
    flood_fill(vmap, row-1, column, reachable);     // North
    flood_fill(vmap, row, column-1, reachable);     // West
    flood_fill(vmap, row, column+1, reachable);     // East
}

coordinate_node * get_unexplored_coordinates(char vmap[HEIGHT][WIDTH], int row, int column) {
    coordinate_node * reachable;
    reachable = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(reachable);

    flood_fill(vmap, row, column, reachable);

    coordinate_node * unexplored;
    unexplored = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(unexplored);

    coordinate_node * current = reachable;

    while (current != NULL) {
        if (vmap[current->val.x][current->val.y] == '.')
            push_coordinate_list(unexplored, current->val);
        current = current->next;
    }

    return unexplored;
}

int check_valid_coordinate(char vmap[HEIGHT][WIDTH], coordinate_node * explored, Coordinate direction) {
    char coordinateSymbol = vmap[direction.x][direction.y];
    int inExplored = coordinate_list_contains(explored, &direction);
    int validSymbol = coordinateSymbol == 'o' || coordinateSymbol == '.';
    return !inExplored && validSymbol;
}

coordinate_node * get_nearest_unexplored(char vmap[HEIGHT][WIDTH], coordinate_node * unexploredCoordinates, int row, int column) {
    // Initial coordinate
    Coordinate c = {row, column};

    // Initial path
    coordinate_node * cl;
    cl = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(cl);
    push_coordinate_list(cl, c);

    // Initial path list
    path_node * paths;
    paths = (path_node *) malloc(sizeof(path_node));
    new_path_list(paths);
    push_path_list(paths, cl);

    coordinate_node * explored;
    explored = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(explored);

    while (!is_coordinate_list_empty(unexploredCoordinates)) {
        path_node * newPaths;
        newPaths = (path_node *) malloc(sizeof(path_node));
        new_path_list(newPaths);

        path_node * p = paths;

        while (p != NULL) {
            Coordinate * last = coordinate_list_last(p->val);
            int currRow = last->x;
            int currCol = last->y;

            Coordinate south = {currRow+1, currCol};
            Coordinate north = {currRow-1, currCol};
            Coordinate west = {currRow, currCol-1};
            Coordinate east = {currRow, currCol+1};

            if (coordinate_list_contains(unexploredCoordinates, &south)) {
                push_coordinate_list(p->val, south);
                return p->val;
            }
            if (coordinate_list_contains(unexploredCoordinates, &north)) {
                push_coordinate_list(p->val, north);
                return p->val;
            }
            if (coordinate_list_contains(unexploredCoordinates, &west)) {
                push_coordinate_list(p->val, west);
                return p->val;
            }
            if (coordinate_list_contains(unexploredCoordinates, &east)) {
                push_coordinate_list(p->val, east);
                return p->val;
            }

            if (check_valid_coordinate(vmap, explored, south)) {
                push_coordinate_list(explored, south);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, south);
                push_path_list(newPaths, newP);
            }
            if (check_valid_coordinate(vmap, explored, north)) {
                push_coordinate_list(explored, north);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, north);
                push_path_list(newPaths, newP);
            }
            if (check_valid_coordinate(vmap, explored, west)) {
                push_coordinate_list(explored, west);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, west);
                push_path_list(newPaths, newP);
            }
            if (check_valid_coordinate(vmap, explored, east)) {
                push_coordinate_list(explored, east);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, east);
                push_path_list(newPaths, newP);
            }

            p = p->next;
        }
        paths = newPaths;
    }
    coordinate_node * empty;
    empty = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(empty);
    return empty;
}

char * parse_path(coordinate_node * path) {
    static char seq [] = "";

    coordinate_node * current = path;

    while (current->next != NULL) {

        int pr = current->val.x;
        int pc = current->val.y;
        int qr = current->next->val.x;
        int qc = current->next->val.y;

        int r = qr - pr;
        int c = qc - pc;

        if (r == -1)
            strcat(seq, "W");
        else if (c == -1)
            strcat(seq, "A");
        else if (r == 1)
            strcat(seq, "S");
        else if (c == 1)
            strcat(seq, "D");

        current = current->next;
    }
    return seq;
}

coordinate_node * find_path(char vmap[HEIGHT][WIDTH], Coordinate s, Coordinate t) {
    coordinate_node * unexplored;
    unexplored = (coordinate_node *) malloc(sizeof(coordinate_node));
    unexplored->val = t;
    unexplored->next = NULL;

    int row = s.x;
    int column = s.y;

    return get_nearest_unexplored(vmap, unexplored, row, column);
}

void move_robot_in_map(char vmap[HEIGHT][WIDTH], int id, Coordinate s, Coordinate t) {
    coordinate_node * path = find_path(vmap, s, t);
    coordinate_node * current = path;

    char idc = (char) (id - 208);

    while (current != NULL) {
        vmap[current->val.x][current->val.y] = idc;
        current = current->next;
    }
}


void robot_moved_in_map(char vmap[HEIGHT][WIDTH], int id, Coordinate target) {
    char idc = (char) (id - 208);

    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++) {
            Coordinate c = {i, j};
            if (vmap[i][j] == idc && !compare_coordinate(&c, &target))
                vmap[i][j] = 'o';
        }
}

