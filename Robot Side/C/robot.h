#ifndef ROBOT_H
#define ROBOT_H

#include "types.h"


int can_free(void);
int can_work(void);
int get_my_id(void);
void go_done(void);
void go_free(void);
void go_work(void);
void init_robots(void);
void new_robot(int id, Coordinate * coordinate);
Robot robots_peek(robot_node * head);
void set_my_id(int id);
Robot get_robot_with_index(int idx);
void push_to_free_robots_list(Robot robot);


#endif
