#ifndef ROBOT_H
#define ROBOT_H

#include "types.h"

/**
 * Get the ID of the robot
 * @return The ID of the robot
 */
int get_my_id(void);

/**
 * Set the ID of the robot
 * @param id The ID to be set
 */
void set_my_id(int id);

/**
 * Initialize lists of robots
 */
void init_robots(void);

/**
 * Add a new robot to the list
 * @param id The ID of the new robot
 * @param coordinate The coordinate of the new robot
 */
void new_robot(int id, Coordinate * coordinate);

/**
 * Get a robot from the list by index
 * @param idx The index of the list
 * @return A robot in the certain index
 */
Robot * get_robot_with_index(int idx);

/**
 * Get the first robot in the list
 * @param head Robot list
 * @return The first robot in the list
 */
Robot * robots_peek(robot_node * head);

/**
 * Push a robot to the free robots list
 * @param robot
 */
void push_to_free_robots_list(Robot * robot);

/**
 * Checks whether the robot is free and can go to work
 * @return 1 if the robot is a ble to work, 0 otherwise
 */
int can_work(void);

/**
 * Pop the first robot from the free robots list
 */
void go_work(void);

#endif //ROBOT_H
