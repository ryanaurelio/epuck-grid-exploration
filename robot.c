#include <stdio.h>
#include <stdlib.h>

#include "robot.h"
#include "types.h"
#include "dynamic_map.h"


// ID of the robot
int ID;

int get_my_id(void) {
    // Returns the ID of the robot
    return ID;
}

void set_my_id(int id) {
    // Set the ID of the robot
    ID = id;
}

void init_robots(void) {
    // Initialize robot lists
    freeRobots = (robot_node *) malloc(sizeof(robot_node));
    new_robot_list(freeRobots);
    robots = (robot_node *) malloc(sizeof(robot_node));
    new_robot_list(robots);
}

void new_robot(int id, Coordinate * coordinate) {
    // Initialize a new robot
    Robot * robot;
    robot = (Robot*) malloc(sizeof(Robot));
    robot -> id = id;
    robot -> coordinate = coordinate;
    robot -> direction = UP;

    // Push the robot to both lists
    push_robot_list(freeRobots, robot);
    push_robot_list(robots, robot);
}

Robot * get_robot_with_index(int idx) {
    // Get the robot at certain index
    robot_node * current = robots;
    while (current != NULL) {
        if (idx-- == 0)
            return current->robot;
        current = current->next;
    }

    // Return an empty robot if not found
    Robot * r = (Robot*) malloc(sizeof(Robot));
    r -> id = 0;
    r -> coordinate = NULL;
    r -> direction = UP;
    return r;
}

Robot * robots_peek(robot_node * head) {
    // Returns the first robot
    return head->robot;
}

void push_to_free_robots_list(Robot * robot) {
    // Push a robot to the end of the free robots list
    push_robot_list(freeRobots, robot);
}

int can_work(void) {
    // Checks whether the first robot in the free robots list equal to the robot's ID
    return robots_peek(freeRobots) -> id == ID;
}

//assuming that all robots hear the same sound every time.
void go_work(void) {
    // Remove the first robot in the free robots list
    pop_robot_list(&freeRobots);
}
