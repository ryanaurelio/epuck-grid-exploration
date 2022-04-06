#include <stdio.h>
#include <stdlib.h>

#include "robot.h"
#include "types.h"
#include "dynamic_map.h"


robot_node* freeRobots;
robot_node* robots;

int ID;

void init_robots(void) {
    freeRobots = (robot_node *) malloc(sizeof(robot_node));
    new_robot_list(freeRobots);

    robots = (robot_node *) malloc(sizeof(robot_node));
    new_robot_list(robots);
}

Robot * robots_peek(robot_node * head) {
    return head->robot;
}

void new_robot(int id, Coordinate * coordinate) {
    Robot * robot;
    robot = (Robot*) malloc(sizeof(Robot));
    robot -> id = id;
    robot -> coordinate = coordinate;
    robot -> direction = UP;
    push_robot_list(freeRobots, robot);
    push_robot_list(robots, robot);
}

void set_my_id(int id) {
    ID = id;
}

int get_my_id(void) {
    return ID;
}

int can_work(void) {
    return robots_peek(freeRobots) -> id == ID;
}

//assuming that all robots hear the same sound every time.
void go_work(void) {
    pop_robot_list(&freeRobots);
}

Robot * get_robot_with_index(int idx) {
    robot_node * current = robots;

    while (current != NULL) {
        if (idx-- == 0)
            return current->robot;
        current = current->next;
    }

    Robot * r = (Robot*) malloc(sizeof(Robot));
    r -> id = 0;
    r -> coordinate = NULL;
    r -> direction = UP;
    return r;
}

void push_to_free_robots_list(Robot * robot) {
	push_robot_list(freeRobots, robot);
}
