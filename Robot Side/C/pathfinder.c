#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "types.h"
#include "robot.h"
#include "move.h"
//#include "leds.h"

void flood_fill(char vmap[HEIGHT][WIDTH], int row, int column, coordinate_node * reachable) {
    char symbol = vmap[row][column];
    Coordinate c = {row, column};
    char id = (char) (get_my_id() - 208);
    if ((symbol != '.' && symbol != 'o' && symbol != id) || coordinate_list_contains(reachable, &c))
//    if ((symbol == 'x') || coordinate_list_contains(reachable, &c))
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

void parse_path(coordinate_node * path, Robot * robot) {
    char * seq = (char*) malloc(HEIGHT * WIDTH * sizeof(char));
    seq[0] = '0';
    coordinate_node * current = path;
    int len = 0;
    while (current->next != NULL) {

        int pr = current->val.x;
        int pc = current->val.y;
        int qr = current->next->val.x;
        int qc = current->next->val.y;

        int r = qr - pr;
        int c = qc - pc;

        if (r == -1)
            seq[len] = 'W';
        else if (c == -1)
            seq[len] = 'A';
        else if (r == 1)
            seq[len] = 'S';
        else if (c == 1)
            seq[len] = 'D';

        current = current->next;
        len++;
    }

    int speed = 550;

    for (int i = 0; i < len; i++) {
        char movement = seq[i];
        switch(movement){
            case 'W':
                switch(robot -> direction) {
                case UP:
                    break;
                case DOWN:
                    turnLeft(speed);
                    turnLeft(speed);
                    break;
                case LEFT:
                    turnRight(speed);
                    break;
                case RIGHT:
                    turnLeft(speed);
                    break;
                }
                moveForward(speed);
                robot -> direction = UP;
                break;
            case 'S':
                switch(robot -> direction) {
                case UP:
                    turnLeft(speed);
                    turnLeft(speed);
                    break;
                case DOWN:
                    break;
                case LEFT:
                    turnLeft(speed);
                    break;
                case RIGHT:
                    turnRight(speed);
                    break;
                }
                moveForward(speed);
                robot -> direction = DOWN;
                break;
            case 'D':
                switch(robot -> direction) {
                case UP:
                    turnRight(speed);
                    break;
                case DOWN:
                    turnLeft(speed);
                    break;
                case LEFT:
                    turnLeft(speed);
                    turnLeft(speed);
                    break;
                case RIGHT:
                    break;
                }
                moveForward(speed);
                robot -> direction = RIGHT;
                break;
            case 'A':
                switch(robot -> direction) {
                case UP:
                    turnLeft(speed);
                    break;
                case DOWN:
                    turnRight(speed);
                    break;
                case LEFT:
                    break;
                case RIGHT:
                    turnLeft(speed);
                    turnLeft(speed);
                    break;
                }
                moveForward(speed);
                robot -> direction = LEFT;
                break;
        }
    }
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
    if (compare_coordinate(&s, &t))
        return;


    coordinate_node * path = find_path(vmap, s, t);
    coordinate_node * current = path;

    char idc = (char) (id - 208);

    while (current != NULL) {
        vmap[current->val.x][current->val.y] = idc;
        current = current->next;
    }

    if(get_my_id() != id) {
        return;
    }
    Robot * r = get_robot_with_index(id - 1);
    parse_path(path, r);
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

Coordinate * get_nearest_coordinate(char vmap[HEIGHT][WIDTH], int row, int column) {
    coordinate_node * unexplored;
    unexplored = get_unexplored_coordinates(vmap, row, column);

    if(is_coordinate_list_empty(unexplored)) {
        Coordinate * c = (Coordinate *) malloc(sizeof(Coordinate));
        c -> x = get_robot_with_index(get_my_id() - 1) -> coordinate -> x;
        c -> y = get_robot_with_index(get_my_id() - 1) -> coordinate -> y;
        return c;
    }

    return coordinate_list_last(get_nearest_unexplored(vmap, unexplored, row, column));

}
