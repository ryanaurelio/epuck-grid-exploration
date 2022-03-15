#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>

#include "robot.h";

// Robot list
typedef struct r_node {
    Robot robot;
    struct r_node * next;
} robot_node;

int is_robot_list_empty(robot_node * head);
void new_robot_list(robot_node * head);
Robot pop_robot_list(robot_node ** head);
void print_robot_list(robot_node * head);
void push_robot_list(robot_node * head, Robot val);

// Coordinate
typedef struct {
    int x;
    int y;
} Coordinate;

int compare_coordinate(Coordinate * c1, Coordinate * c2);
void print_coordinate(Coordinate * c);

// Coordinate list
typedef struct c_node {
    Coordinate val;
    struct c_node * next;
} coordinate_node;

int coordinate_list_contains(coordinate_node * head, Coordinate * c);
Coordinate * coordinate_list_last(coordinate_node * head);
int is_coordinate_list_empty(coordinate_node * head);
void new_coordinate_list(coordinate_node * head);
void print_coordinate_list(coordinate_node * head);
void push_coordinate_list(coordinate_node * head, Coordinate val);

// Path list
typedef struct p_node {
    coordinate_node * val;
    struct p_node * next;
} path_node;

int is_path_list_empty(path_node * head);
void new_path_list(path_node * head);
void print_path_list(path_node * head);
void push_path_list(path_node * head, coordinate_node * val);

#endif //TYPES_H
