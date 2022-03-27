#include <stdlib.h>
#include <string.h>

#include "dmap_pathfinder.h"
#include "dynamic_map.h"
#include "robot.h"
#include "move.h"

void dfs_unexplored(dmap map, int x, int y, coordinate_node * explored, Coordinate * unexplored, int * is_found) {
    // End of recursion when the unexplored coordinate has been found
    if (* is_found) return;

    // Current coordinate and symbol to be evaluated
    char symbol = get_symbol_dmap(&map, x, y);
    Coordinate c = {x, y};

    // We save the unexplored coordinate in 'unexplored' and set 'is_found' to 1 if an unexplored grid is found
    if (symbol == '.') {
        unexplored->x = c.x;
        unexplored->y = c.y;
        * is_found = 1;
        return;
    }

    // Get the ID of the robot and cast to char
    char id = (char) (get_my_id() - 208);

    // Stop the recursion for current branch if an invalid symbol is found ('u'/'x'/'0')
    if (!(symbol == 'o' || symbol == id) || coordinate_list_contains(explored, &c)) return;

    // Prevent the dfs to explore the explored grid again
    push_coordinate_list(explored, c);

    // Next recursion exploring the direction in order
    dfs_unexplored(map, x, y-1, explored, unexplored, is_found);     // South
    dfs_unexplored(map, x, y+1, explored, unexplored, is_found);     // North
    dfs_unexplored(map, x-1, y, explored, unexplored, is_found);     // West
    dfs_unexplored(map, x+1, y, explored, unexplored, is_found);     // East
}

int check_valid_coordinate_dmap(dmap map, coordinate_node * explored, Coordinate direction) {
    // The coordinate at the next direction
    char coordinateSymbol = get_symbol_dmap(&map, direction.x, direction.y);

    // Indicator whether the coordinate has been explored
    int inExplored = coordinate_list_contains(explored, &direction);

    // Check whether the robot can stand on the coordinate
    int validSymbol = coordinateSymbol == 'o' || coordinateSymbol == '.';

    return !inExplored && validSymbol;
}

coordinate_node * get_nearest_unexplored_dmap(dmap map, coordinate_node * unexploredCoordinates, int x, int y) {
    // Initial coordinate
    Coordinate c = {x, y};

    // Path consist of only a coordinate
    coordinate_node * cl;
    cl = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(cl);
    push_coordinate_list(cl, c);

    // Initial path list that consists only a path with only 1 coordinate
    path_node * paths;
    paths = (path_node *) malloc(sizeof(path_node));
    new_path_list(paths);
    push_path_list(paths, cl);

    // A list of explored grids to prevent checking the grid again
    coordinate_node * explored;
    explored = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(explored);

    // Search for a path as long as there is an unexplored coordinate using a breadth first search
    while (!is_coordinate_list_empty(unexploredCoordinates)) {
        // A list of paths with the same depth
        path_node * newPaths;
        newPaths = (path_node *) malloc(sizeof(path_node));
        new_path_list(newPaths);

        // Initial path list
        path_node * p = paths;

        // Add the next coordinate according to direction for every path in path list
        while (p != NULL) {
            // Take the last coordinate of the current path
            Coordinate * last = coordinate_list_last(p->val);
            int cx = last->x;
            int cy = last->y;

            // Next coordinate directions
            Coordinate south = {cx, cy-1};
            Coordinate north = {cx, cy+1};
            Coordinate west = {cx-1, cy};
            Coordinate east = {cx+1, cy};

            // If an unexplored coordinate is found, return current path
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

            // Construct a new path for each valid coordinates
            if (check_valid_coordinate_dmap(map, explored, south)) {
                push_coordinate_list(explored, south);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, south);
                push_path_list(newPaths, newP);
            }
            if (check_valid_coordinate_dmap(map, explored, north)) {
                push_coordinate_list(explored, north);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, north);
                push_path_list(newPaths, newP);
            }
            if (check_valid_coordinate_dmap(map, explored, west)) {
                push_coordinate_list(explored, west);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, west);
                push_path_list(newPaths, newP);
            }
            if (check_valid_coordinate_dmap(map, explored, east)) {
                push_coordinate_list(explored, east);
                coordinate_node * newP = copy_coordinate_list(p->val);
                push_coordinate_list(newP, east);
                push_path_list(newPaths, newP);
            }
            p = p->next;
        }
        // Update the path list with the new longer paths
        paths = newPaths;
    }

    // Return empty list if there are no paths found
    coordinate_node * empty;
    empty = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(empty);
    return empty;
}

void move_path(Robot * robot, grid * seq) {
    // The speed of the robot
    int speed = 550;

    // Current movement sequence
    grid * current = seq;
    while (current != NULL) {
        // Move the robot according to the sequence
        char movement = current->symbol;
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
        current = current->next;
    }
}

void parse_path_dmap(coordinate_node * path, Robot * robot) {
    // Initialize empty sequence
    grid * seq = (grid *) malloc(sizeof(grid));
    new_row(seq, 0, 0);

    // Determine the direction for every adjacent pair of coordinates
    coordinate_node * current = path;
    while (current->next != NULL) {
        // Pair of coordinates
        int px = current->val.x;
        int py = current->val.y;
        int qx = current->next->val.x;
        int qy = current->next->val.y;

        // Coordinate differences
        int mx = qx - px;
        int my = qy - py;

        // Determine the direction
        if (my == 1)
            push_back_symbol(seq, 'W');
        else if (mx == -1)
            push_back_symbol(seq, 'A');
        else if (my == -1)
            push_back_symbol(seq, 'S');
        else if (mx == 1)
            push_back_symbol(seq, 'D');

        current = current->next;
    }
    // Call 'move_path' to move the physical robot
    move_path(robot, seq);
}

void move_robot_in_dmap(dmap map, int id, Coordinate s, Coordinate t) {
    // Return if the starting coordinate is the same as the target coordinate
    if (compare_coordinate(&s, &t)) return;

    // Find a path from the starting coordinate to the target coordinate
    coordinate_node * path = find_path_dmap(map, s, t);

    // Cast ID to char
    char idc = (char) (id - 208);

    // Set every symbol on the map in the path to the ID of the robot
    coordinate_node * current = path;
    while (current != NULL) {
        set_symbol_dmap(&map, current->val.x, current->val.y, idc);
        current = current->next;
    }

    // Return if the IDs are not equal
    if (get_my_id() != id) return;

    // Move the physical robot in map
    Robot * r = get_robot_with_index(id - 1);
    parse_path_dmap(path, r);
}

void robot_moved_in_dmap(dmap map, int id, Coordinate target) {
    // Cast ID to char
    char idc = (char) (id - 208);

    // Set every symbol 'id' to explored 'o' except the target coordinate
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

coordinate_node * find_path_dmap(dmap map, Coordinate s, Coordinate t) {
    // Initialize path
    coordinate_node * unexplored;
    unexplored = (coordinate_node *) malloc(sizeof(coordinate_node));
    unexplored->val = t;
    unexplored->next = NULL;

    // Starting coordinates
    int x = s.x;
    int y = s.y;

    // Get a path (sequence of coordinates) to the nearest unexplored coordinate
    return get_nearest_unexplored_dmap(map, unexplored, x, y);
}


Coordinate * get_nearest_coordinate_dmap(dmap map, int x, int y) {
    // Initialize reachable grids
    coordinate_node * reachable;
    reachable = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(reachable);

    // Unexplored coordinate
    Coordinate * unexplored = (Coordinate *) malloc(sizeof(Coordinate));
    unexplored->x = get_robot_with_index(get_my_id() - 1)->coordinate->x;
    unexplored->y = get_robot_with_index(get_my_id() - 1)->coordinate->y;

    // An indicator when the unexplored coordinate has been found
    int * is_found = (int *) malloc(sizeof(int));
    * is_found = 0;

    // Find an unexplored grid
    dfs_unexplored(map, x, y, reachable, unexplored, is_found);

    // Clean up memory
    free(reachable);
    free(is_found);

    return unexplored;
}
