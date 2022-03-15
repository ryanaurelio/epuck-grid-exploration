
#include <stdio.h>
#include <stdlib.h>

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

node_t* freeRobots;
node_t* workingRobots;
uint ID;

void robots_push(node_t * head, Robot val) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->robot = val;
    current->next->next = NULL;
}

Robot robots_pop(node_t ** head) {
    Robot retval;
    node_t * next_node = NULL;

//    if (*head == NULL) {
//        return -1;
//    }

    next_node = (*head)->next;
    retval = (*head)->robot;
    free(*head);
    *head = next_node;

    return retval;
}

Robot robots_peek(node_t ** head) {

//    if (*head == NULL) {
//        return -1;
//    }
    return ((*head)->robot);
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

void new_robot(uint id) {
	Robot robot;
	robot.id = id;
	robot.status = FREE;
	robots_push(freeRobots, robot);
}

void set_my_id(uint id) {
	ID = id;
}

int get_my_id(void) {
	return ID;
}

int can_work(void) {
	return robots_peek(&workingRobots).id == ID;
}

