#ifndef ROBOT_H
#define ROBOT_H

#include "types.h"

int can_free(int id);
int can_work(int id);
int get_my_id(void);
void go_done(void);
void go_free(void);
void go_work(void);
void init_robots(void);
void new_robot(int id);
Robot robots_peek(robot_node * head);
void set_my_id(int id);

// Test
robot_node * get_free();
robot_node * get_work();
robot_node * get_done();

#endif
