#ifndef ROBOT_H
#define ROBOT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	WORKING,
	FREE
} status;

typedef struct {
    uint id;
    status status;
} Robot;

typedef struct node{
	Robot robot;
	struct node* next;
} node_t;

void robots_push(node_t * head, Robot val);
Robot robots_pop(node_t ** head);
void new_robot(uint id);
void set_my_id(uint id);
void free_robots(node_t ** head);
int get_my_id(void);
int can_work(void);
int can_free(void);
void go_work(void);
void go_free(void);
void go_done(void);
void init_robots(void);

#ifdef __cplusplus
}
#endif

#endif
