#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "pathfinder.h"
#include "types.h"

int main() {
    // Map initialization
    char vmap[HEIGHT][WIDTH];
    construct_map(vmap);
    print_map(vmap);

    // Map set symbols
    setExplored(vmap, 3,3);
    print_map(vmap);

    // Coordinate
    Coordinate * c1, * c2;
    c1 = (Coordinate *) malloc(sizeof(Coordinate));
    c1->x = 2;
    c1->y = 3;
    c2 = (Coordinate *) malloc(sizeof(Coordinate));
    c2->x = 5;
    c2->y = 7;
    print_coordinate(c1);
    printf("\n");

    // Coordinate list
    coordinate_node * head;
    head = (coordinate_node *) malloc(sizeof(coordinate_node));
    head->val = * c1;
    head->next = NULL;
    print_coordinate_list(head);
    printf("\n");

    // List push_coordinate_list
    push_coordinate_list(head, *c2);
    print_coordinate_list(head);
    printf("\n");

    // Compare coordinate
    Coordinate * c3;
    c3 = (Coordinate *) malloc(sizeof(Coordinate));
    c3->x = 5;
    c3->y = 7;
    printf("%d\n", compare_coordinate(c1, c3));
    printf("%d\n", compare_coordinate(c2, c3));

    // Empty list
    coordinate_node * empty;
    empty = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(empty);
    print_coordinate_list(empty);
    printf("\n");
    printf("%d\n", is_coordinate_list_empty(empty));
    push_coordinate_list(empty, *c2);
    print_coordinate_list(empty);
    printf("\n");
    push_coordinate_list(empty, *c1);
    print_coordinate_list(empty);
    printf("\n");

    // coordinate_list_contains
    Coordinate * c4;
    c4 = (Coordinate *) malloc(sizeof(Coordinate));
    c4->x = 4;
    c4->y = 4;
    printf("%d\n", coordinate_list_contains(empty, c1));
    printf("%d\n", coordinate_list_contains(empty, c4));

    // Flood fill
    construct_map(vmap);
    setObstacle(vmap, 2, 3);
    print_map(vmap);
    coordinate_node * reachable;
    reachable = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(reachable);
    flood_fill(vmap, 2, 5, reachable);
    print_coordinate_list(reachable);
    printf("\n");

    // get_unexplored_grids
    setExplored(vmap, 2, 5);
    coordinate_node * unexplored;
    unexplored = get_unexplored_coordinates(vmap, 2, 5);
    print_coordinate_list(unexplored);
    printf("\n");

    // Path list
    path_node * p;
    p = (path_node *) malloc(sizeof(path_node));
    new_path_list(p);
    print_path_list(p);
    printf("\n");

    // Push path list
    push_path_list(p, unexplored);
    print_path_list(p);
    printf("\n");
    push_path_list(p, empty);
    print_path_list(p);
    printf("\n");

    // Get nearest unexplored
    coordinate_node * nearUnexplored;
    nearUnexplored =  get_nearest_unexplored(vmap, unexplored, 1, 5);
    print_coordinate_list(nearUnexplored);
    printf("\n");
    print_coordinate_list(unexplored);
    printf("\n");

    // Parse path
    char * path = parse_path(nearUnexplored);
    printf("%s\n", path);

    // Robot list
    robot_node * r;
    r = (robot_node *) malloc(sizeof(robot_node));
    new_robot_list(r);
    print_robot_list(r);
    printf("\n");

    Robot r1;
    r1.id = 1;
    Robot r2;
    r2.id = 2;
    push_robot_list(r, r1);
    push_robot_list(r, r2);

    print_robot_list(r);
    printf("\n");

    Robot rt1 = pop_robot_list(&r);
    print_robot_list(r);
    printf("\n");

    Robot rt2 = pop_robot_list(&r);
    print_robot_list(r);
    printf("\n");

    return 0;
}
