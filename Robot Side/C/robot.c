#include <stdio.h>
#include <stdlib.h>

#include "robot.h"
#include "types.h"

robot_node* freeRobots;
robot_node* robots;
robot_node* workingRobots;
robot_node* doneRobots;

int ID;

void init_robots(void) {
    freeRobots = (robot_node *) malloc(sizeof(robot_node));
    new_robot_list(freeRobots);

    workingRobots = (robot_node *) malloc(sizeof(robot_node));
    new_robot_list(workingRobots);

    doneRobots = (robot_node *) malloc(sizeof(robot_node));
    new_robot_list(doneRobots);

    robots = (robot_node *) malloc(sizeof(robot_node));
    new_robot_list(robots);
}

Robot robots_peek(robot_node * head) {
    return head->robot;
}

void new_robot(int id, Coordinate * coordinate) {
    Robot robot;
    robot.id = id;
    robot.coordinate = coordinate;
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
    return robots_peek(freeRobots).id == ID;
}

//assuming that all robots hear the same sound every time.
void go_work(void) {
    pop_robot_list(&freeRobots);
}

robot_node* get_list_robot(void) {
	return robots;
}

void push_to_free_robots_list(Robot robot) {
	push_robot_list(freeRobots, robot);
}

Robot get_robot_with_index(robot_node * robot_list, int idx) {
    robot_node * current = robot_list;

    while (current != NULL) {
        if (idx-- == 0)
            return current->robot;
        current = current->next;
    }

    Robot r = {0, NULL};
    return r;
}
