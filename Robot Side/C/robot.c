#include <stdio.h>
#include <stdlib.h>

typedef enum{
    WORKING,
    FREE
} status;

typedef struct {
    int id;
    status status;
} Robot;

typedef struct node{
    Robot robot;
    struct node* next;
} node_t;

node_t* freeRobots = NULL;
node_t* workingRobots = NULL;
int ID;
int zero_in_free = 1;
int zero_in_working = 1;

void init_robots() {
    Robot robot;
    robot.id = 0;
    robot.status = FREE;
    freeRobots = (node_t *) malloc(sizeof(node_t));
    freeRobots->robot = robot;
    freeRobots->next = NULL;
    workingRobots = (node_t *) malloc(sizeof(node_t));
    workingRobots->next = NULL;
    workingRobots -> robot = robot;
}

Robot robots_peek(node_t ** head) {

//    if (*head == NULL) {
//        return -1;
//    }
    return ((*head)->robot);
}

void robots_push(node_t * head, Robot val) {
    node_t * current = head;

    while (current->next != NULL) {
        current = head->next;
    }
    /* now we can add a new variable */
    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->robot = val;
    current->next->next = NULL;

}

void check_list() {
    if(robots_peek(&freeRobots).id == 0)
        zero_in_free = 1;

    if(robots_peek(&workingRobots).id == 0)
        zero_in_working = 1;
}


Robot robots_pop(node_t ** head) {
    Robot retval;
    node_t * next_node = NULL;

//    if (*head == NULL) {
//        return -1;
//    }

    next_node = (*head)->next;
    //this was a buffer so that there r no segmentation leak. If better fix is found, feel free to change
    //this was used to add buffer
    if (next_node == NULL) {
        Robot robot;
        robot.id = 0;
        robots_push(*head, robot);
        next_node = (*head)->next;
    }
    retval = (*head)->robot;
    free(*head);
    *head = next_node;
    check_list();
    return retval;
}

void free_robots(node_t ** head) {
    node_t * next_node = NULL;
    next_node = (*head) -> next;
    while(next_node != NULL) {
        free(*head);
        *head = next_node;
        next_node = (*head) -> next;
    }
}

void new_robot(int id) {
    Robot robot;
    robot.id = id;
    robot.status = FREE;
    robots_push(freeRobots, robot);
    if(zero_in_free) {
        robots_pop(&freeRobots);
        zero_in_free = 0;
    }
}

void set_my_id(int id) {
    ID = id;
}

int get_my_id(void) {
    return ID;
}

int can_work(void) {
    return robots_peek(&workingRobots).id == ID;
}

//assuming that all robots hear the same sound every time.
void go_work() {
	if (id == NULL) {
		id = ID;
	}
    robots_push(workingRobots, robots_pop(&freeRobots));
    if(zero_in_working) {
        robots_pop(&workingRobots);
        zero_in_working = 0;
    }
}

void go_free() {
    robots_push(freeRobots, robots_pop(&workingRobots));
    if(zero_in_free) {
        robots_pop(&freeRobots);
        zero_in_free = 0;
    }
}
