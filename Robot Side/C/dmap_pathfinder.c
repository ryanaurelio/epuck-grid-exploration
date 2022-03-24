#include <stdlib.h>
#include <string.h>

#include "dmap_pathfinder.h"
#include "map.h"
#include "robot.h"
#include "move.h"
//#include "leds.h"

void flood_fill_dmap(dmap map, int x, int y, coordinate_node * reachable) {
    char symbol = get_symbol_dmap(&map, x, y);
    Coordinate c = {x, y};
    char id = (char) (get_my_id() - 208);
    if (!(symbol == '.' || symbol == 'o' || symbol == id) || symbol == '0' || coordinate_list_contains(reachable, &c))
        return;

    push_coordinate_list(reachable, c);
    flood_fill_dmap(map, x, y-1, reachable);     // South
    flood_fill_dmap(map, x, y+1, reachable);     // North
    flood_fill_dmap(map, x-1, y, reachable);     // West
    flood_fill_dmap(map, x+1, y, reachable);     // East
}

coordinate_node * get_unexplored_coordinates_dmap(dmap map, int x, int y) {
    coordinate_node * reachable;
    reachable = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(reachable);

    flood_fill_dmap(map, x, y, reachable);

    coordinate_node * unexplored;
    unexplored = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(unexplored);

    coordinate_node * current = reachable;

    while (current != NULL) {
        if (get_symbol_dmap(&map, current->val.x, current->val.y) == '.')
            push_coordinate_list(unexplored, current->val);
        current = current->next;
    }

    return unexplored;
}

int check_valid_coordinate(dmap map, coordinate_node * explored, Coordinate direction) {
    char coordinateSymbol = get_symbol_dmap(&map, direction.x, direction.y);
    int inExplored = coordinate_list_contains(explored, &direction);
    int validSymbol = coordinateSymbol == 'o' || coordinateSymbol == '.';
    return !inExplored && validSymbol;
}

coordinate_node * get_nearest_unexplored_dmap(dmap map, coordinate_node * unexploredCoordinates, int x, int y) {
    // Initial coordinate
    Coordinate c = {x, y};

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

            if (check_valid_coordinate(map, explored, south)) {
                push_coordinate_list(explored, south);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, south);
                push_path_list(newPaths, newP);
            }
            if (check_valid_coordinate(map, explored, north)) {
                push_coordinate_list(explored, north);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, north);
                push_path_list(newPaths, newP);
            }
            if (check_valid_coordinate(map, explored, west)) {
                push_coordinate_list(explored, west);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, west);
                push_path_list(newPaths, newP);
            }
            if (check_valid_coordinate(map, explored, east)) {
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

void move_path(Robot * robot, char * seq, int len) {
//    int speed = 550;
//
//    for (int i = 0; i < len; i++) {
//        char movement = seq[i];
//        switch(movement){
//            case 'W':
//                switch(robot -> direction) {
//                case UP:
//                    break;
//                case DOWN:
//                    turnLeft(speed);
//                    turnLeft(speed);
//                    break;
//                case LEFT:
//                    turnRight(speed);
//                    break;
//                case RIGHT:
//                    turnLeft(speed);
//                    break;
//                }
//                moveForward(speed);
//                robot -> direction = UP;
//                break;
//            case 'S':
//                switch(robot -> direction) {
//                case UP:
//                    turnLeft(speed);
//                    turnLeft(speed);
//                    break;
//                case DOWN:
//                    break;
//                case LEFT:
//                    turnLeft(speed);
//                    break;
//                case RIGHT:
//                    turnRight(speed);
//                    break;
//                }
//                moveForward(speed);
//                robot -> direction = DOWN;
//                break;
//            case 'D':
//                switch(robot -> direction) {
//                case UP:
//                    turnRight(speed);
//                    break;
//                case DOWN:
//                    turnLeft(speed);
//                    break;
//                case LEFT:
//                    turnLeft(speed);
//                    turnLeft(speed);
//                    break;
//                case RIGHT:
//                    break;
//                }
//                moveForward(speed);
//                robot -> direction = RIGHT;
//                break;
//            case 'A':
//                switch(robot -> direction) {
//                case UP:
//                    turnLeft(speed);
//                    break;
//                case DOWN:
//                    turnRight(speed);
//                    break;
//                case LEFT:
//                    break;
//                case RIGHT:
//                    turnLeft(speed);
//                    turnLeft(speed);
//                    break;
//                }
//                moveForward(speed);
//                robot -> direction = LEFT;
//                break;
//        }
//    }
}

void parse_path_dmap(coordinate_node * path, Robot * robot) {
    char * seq = (char*) malloc(HEIGHT * WIDTH * sizeof(char));
    seq[0] = '0';
    coordinate_node * current = path;
    int len = 0;
    while (current->next != NULL) {

        int px = current->val.x;
        int py = current->val.y;
        int qx = current->next->val.x;
        int qy = current->next->val.y;

        int mx = qx - px;
        int my = qy - py;

        if (my == 1)
        	seq[len] = 'W';
        else if (mx == -1)
        	seq[len] = 'A';
        else if (my == -1)
        	seq[len] = 'S';
        else if (mx == 1)
        	seq[len] = 'D';

        current = current->next;
        len++;
    }
    move_path(robot, seq, len);
}

coordinate_node * find_path_dmap(dmap map, Coordinate s, Coordinate t) {
    coordinate_node * unexplored;
    unexplored = (coordinate_node *) malloc(sizeof(coordinate_node));
    unexplored->val = t;
    unexplored->next = NULL;

    int x = s.x;
    int y = s.y;

    return get_nearest_unexplored_dmap(map, unexplored, x, y);
}

void move_robot_in_dmap(dmap map, int id, Coordinate s, Coordinate t) {
    if (compare_coordinate(&s, &t))
        return;

    coordinate_node * path = find_path_dmap(map, s, t);
    coordinate_node * current = path;

    char idc = (char) (id - 208);

    while (current != NULL) {
        set_symbol_dmap(&map, current->val.x, current->val.y, idc);
        current = current->next;
    }

    if (get_my_id() != id)
        return;
    Robot * r = get_robot_with_index(id - 1);
    parse_path_dmap(path, r);
}

void robot_moved_in_dmap(dmap map, int id, Coordinate target) {
    char idc = (char) (id - 208);

    dmap * current_y = &map;
    while (current_y != NULL) {
        grid * current_x = current_y->row;
        while (current_x != NULL) {
            if (current_x->symbol == idc && !compare_coordinate(&current_x->coordinate, &target))
                current_x->symbol = 'o';
            current_x = current_x->next;
        }
        current_y = current_y->next;
    }
}

Coordinate * get_nearest_coordinate_dmap(dmap map, int x, int y) {
	coordinate_node * unexplored;
    unexplored = get_unexplored_coordinates_dmap(map, x, y);

    if(is_coordinate_list_empty(unexplored))
    	return get_robot_with_index(get_my_id()-1)->coordinate;

    return coordinate_list_last(get_nearest_unexplored_dmap(map, unexplored, x, y));
}
