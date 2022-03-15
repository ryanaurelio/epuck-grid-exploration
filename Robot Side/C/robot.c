#include <stdio.h>
#include <stdlib.h>

#include "robot.h"
#include "types.h"

robot_node* freeRobots;
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
}

Robot robots_peek(robot_node * head) {
    return head->robot;
}

void new_robot(int id) {
    Robot robot;
    robot.id = id;
    robot.status = FREE;
    push_robot_list(freeRobots, robot);
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

int can_free(void) {
    return robots_peek(doneRobots).id == ID;
}

//assuming that all robots hear the same sound every time.
void go_work(void) {
    push_robot_list(workingRobots, pop_robot_list(&freeRobots));
}

void go_done(void) {
    push_robot_list(doneRobots, pop_robot_list(&workingRobots));
}

void go_free(void) {
    push_robot_list(freeRobots, pop_robot_list(&doneRobots));
}
