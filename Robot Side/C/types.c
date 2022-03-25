#include <limits.h>

#include "types.h"

// Robot list
int is_robot_list_empty(robot_node * head) {
    return head->robot -> id == 0;
}

void new_robot_list(robot_node * head) {
    Robot * r;
    r = (Robot*) malloc(sizeof(Robot));
    r -> id = 0;


    head->robot = r;
    head->next = NULL;
}

Robot * pop_robot_list(robot_node ** head) {
    if (is_robot_list_empty(*head)) {
        return (*head)->robot;
    } else if ((*head)->next == NULL) {
    	Robot * r = (Robot *) malloc(sizeof(Robot));
        r = (*head)->robot;
        new_robot_list(*head);
        return r;
    } else {

    	Robot * r = (Robot *) malloc(sizeof(Robot));
        r = (*head)->robot;

        robot_node * next_node = NULL;
        next_node = (*head)->next;
        free(*head);
        *head = next_node;

        return r;
    }
}

void print_robot_list(robot_node * head) {
    printf("[");

    robot_node * current = head;

    while (current != NULL) {
        printf("%d, ", current->robot -> id);
        current = current->next;
    }
    printf("]");
}

void push_robot_list(robot_node * head, Robot * val) {
    if (is_robot_list_empty(head))
        head->robot = val;
    else {
        robot_node *current = head;
        while (current->next != NULL) {
            current = current->next;
        }

        current->next = (robot_node *) malloc(sizeof(robot_node));
        current->next->robot = val;
        current->next->next = NULL;
    }
}

// Coordinate
int compare_coordinate(Coordinate * c1, Coordinate * c2) {
    return c1->x == c2->x && c1->y == c2->y;
}

void print_coordinate(Coordinate * c) {
    printf("(%3d, %3d)", c->x, c->y);
}

// Coordinate List
int coordinate_list_contains(coordinate_node * head, Coordinate * c) {
    coordinate_node * current = head;

    while (current != NULL) {
        if (compare_coordinate(&current->val, c))
            return 1;
        current = current->next;
    }

    return 0;
}

int is_coordinate_list_empty(coordinate_node * head) {
    return head->val.x == INT_MIN && head->val.y == INT_MIN && head->next == NULL;
}

Coordinate * coordinate_list_last(coordinate_node * head) {
    coordinate_node * current = head;

    while (current->next != NULL)
        current = current->next;

    return &current->val;
}

void new_coordinate_list(coordinate_node * head) {
    Coordinate * c;
    c = (Coordinate *) malloc(sizeof(Coordinate));
    c->x = INT_MIN;
    c->y = INT_MIN;
    head->val = *c;
    head->next = NULL;
}

void push_coordinate_list(coordinate_node * head, Coordinate val) {
    if (is_coordinate_list_empty(head))
        head->val = val;
    else {
        coordinate_node *current = head;
        while (current->next != NULL) {
            current = current->next;
        }

        current->next = (coordinate_node *) malloc(sizeof(coordinate_node));
        current->next->val = val;
        current->next->next = NULL;
    }
}

void print_coordinate_list(coordinate_node * head) {
    coordinate_node * current = head;

    printf("[");
    while (current != NULL) {
        print_coordinate(&current->val);
        printf(", ");
        current = current->next;
    }
    printf("]");
}

coordinate_node * copy_coordinate_list(coordinate_node * head) {
    if (head == NULL) {
        return NULL;
    } else {
        coordinate_node * newNode = (coordinate_node *) malloc(sizeof(coordinate_node));
        newNode->val = head->val;
        newNode->next = copy_coordinate_list(head->next);
        return newNode;
    }
}

int coordinate_list_length(coordinate_node * head) {
    if (is_coordinate_list_empty(head))
        return 0;

    coordinate_node * current = head;
    int len = 0;
    while (current != NULL) {
        len++;
        current = current->next;
    }
    return len;
}

// Path list
int is_path_list_empty(path_node * head) {
    return head->val->val.x == INT_MIN && head->val->val.y == INT_MIN && head->next == NULL;
}

void new_path_list(path_node * head) {
    coordinate_node * coordinate_list;
    coordinate_list = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(coordinate_list);
    head->val = coordinate_list;
    head->next = NULL;
}

void print_path_list(path_node * head) {
    path_node * current = head;

    printf("[");
    while (current != NULL) {
        print_coordinate_list(current->val);
        printf(", ");
        current = current->next;
    }
    printf("]");
}

void push_path_list(path_node * head, coordinate_node * val) {
    if (is_path_list_empty(head))
        head->val = val;
    else {
        path_node *current = head;
        while (current->next != NULL) {
            current = current->next;
        }

        current->next = (path_node *) malloc(sizeof(path_node));
        current->next->val = val;
        current->next->next = NULL;
    }
}

void new_sound_list(sound_node *head) {
	head -> sound = -1;
	head -> next = NULL;

}
int is_sound_list_empty(sound_node * head) {
	return (head) -> sound == -1;
}

int pop_sound_list(sound_node ** head) {
    if(is_sound_list_empty(*head)) {
    	return (*head) -> sound;
    }
    else if((*head) -> next == NULL) {
    	int sound = -1;
    	new_sound_list((*head));
    	return sound;
    }
    else {
		sound_node * new_head = (*head) -> next;

		int sound = (*head) -> sound;
		free(*head);
		*head = new_head;
		return sound;
    }
}

void push_sound_list(sound_node ** head, int val) {
	if (is_sound_list_empty((*head))) {
		(*head) -> sound = val;
		return;
	}

	sound_node * new_head = (sound_node *) malloc(sizeof(sound_node));
	new_head -> sound = val;
	new_head -> next = *head;
	(*head) = new_head;
}

