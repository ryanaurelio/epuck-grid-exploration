#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

int compare_coordinate(Coordinate * c1, Coordinate * c2) {
    // Checks whether both coordinate contains the same position
    return c1->x == c2->x && c1->y == c2->y;
}

void print_coordinate(Coordinate * c) {
    // Print a given coordinate in tuple form
    printf("(%3d, %3d)", c->x, c->y);
}

void new_coordinate_list(coordinate_node * head) {
    // Create an empty coordinate list with position 'INT_MIN'
    Coordinate * c;
    c = (Coordinate *) malloc(sizeof(Coordinate));
    c->x = INT_MIN;
    c->y = INT_MIN;
    head->val = *c;
    head->next = NULL;
}

coordinate_node * copy_coordinate_list(coordinate_node * head) {
    // Recursion function to copy all the elements in the list
    if (head == NULL) {
        return NULL;
    } else {
        coordinate_node * newNode = (coordinate_node *) malloc(sizeof(coordinate_node));
        newNode->val = head->val;
        newNode->next = copy_coordinate_list(head->next);
        return newNode;
    }
}

void print_coordinate_list(coordinate_node * head) {
    // Iterate through all the coordinates and print them
    coordinate_node * current = head;
    printf("[");
    while (current != NULL) {
        print_coordinate(&current->val);
        printf(", ");
        current = current->next;
    }
    printf("]");
}

int coordinate_list_length(coordinate_node * head) {
    // Return 0 if list is empty
    if (is_coordinate_list_empty(head))
        return 0;

    // Iterate through all the coordinates while incrementing the length
    int len = 0;
    coordinate_node * current = head;
    while (current != NULL) {
        len++;
        current = current->next;
    }
    return len;
}

Coordinate * coordinate_list_last(coordinate_node * head) {
    // Iterate through the list and return the last element
    coordinate_node * current = head;
    while (current->next != NULL)
        current = current->next;
    return &current->val;
}

int is_coordinate_list_empty(coordinate_node * head) {
    // Checks whether the position is 'INT_MIN'
    return head->val.x == INT_MIN && head->val.y == INT_MIN && head->next == NULL;
}

int coordinate_list_contains(coordinate_node * head, Coordinate * c) {
    // Iterate through all the coordinates and compare whether they are the same
    coordinate_node * current = head;
    while (current != NULL) {
        if (compare_coordinate(&current->val, c))
            return 1;
        current = current->next;
    }
    return 0;
}

void push_coordinate_list(coordinate_node * head, Coordinate val) {
    if (is_coordinate_list_empty(head))
        head->val = val;
    else {
        // Iterate through all the coordinates and add the new coordinate at the end
        coordinate_node *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = (coordinate_node *) malloc(sizeof(coordinate_node));
        current->next->val = val;
        current->next->next = NULL;
    }
}

void new_path_list(path_node * head) {
    // Create a new path list that only contains 1 path
    coordinate_node * coordinate_list;
    coordinate_list = (coordinate_node *) malloc(sizeof(coordinate_node));
    new_coordinate_list(coordinate_list);
    head->val = coordinate_list;
    head->next = NULL;
}

void print_path_list(path_node * head) {
    // Iterate through all the paths in the list and print them
    path_node * current = head;
    printf("[");
    while (current != NULL) {
        print_coordinate_list(current->val);
        printf(", ");
        current = current->next;
    }
    printf("]");
}

int is_path_list_empty(path_node * head) {
    // Checks whether the value of the first element is 'INT_MIN'
    return head->val->val.x == INT_MIN && head->val->val.y == INT_MIN && head->next == NULL;
}

void push_path_list(path_node * head, coordinate_node * val) {
    if (is_path_list_empty(head))
        head->val = val;
    else {
        // Iterate through all the paths and insert the given path at the end of the list
        path_node *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = (path_node *) malloc(sizeof(path_node));
        current->next->val = val;
        current->next->next = NULL;
    }
}

void new_robot_list(robot_node * head) {
    // Create a new robot list that consists only 1 robot with ID 0
    Robot * r;
    r = (Robot*) malloc(sizeof(Robot));
    r -> id = 0;
    head->robot = r;
    head->next = NULL;
}

void print_robot_list(robot_node * head) {
    // Iterate through all the robots and print their ID
    printf("[");
    robot_node * current = head;
    while (current != NULL) {
        printf("%d, ", current->robot -> id);
        current = current->next;
    }
    printf("]");
}

int is_robot_list_empty(robot_node * head) {
    // Checks whether the first robot in the list has ID 0
    return head->robot -> id == 0;
}

void push_robot_list(robot_node * head, Robot * val) {
    if (is_robot_list_empty(head))
        head->robot = val;
    else {
        // Iterate through all the robots and insert the given robot at the end of the list
        robot_node *current = head;
        while (current->next != NULL) {
            current = current->next;
        }

        current->next = (robot_node *) malloc(sizeof(robot_node));
        current->next->robot = val;
        current->next->next = NULL;
    }
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
        // Pop the first robot in the list and change the pointer of the list to the next robot
        Robot * r = (Robot *) malloc(sizeof(Robot));
        r = (*head)->robot;
        robot_node * next_node = NULL;
        next_node = (*head)->next;
        free(*head);
        *head = next_node;
        return r;
    }
}

void new_sound_list(sound_node *head) {
    // Create a new sound list that consists of 1 sound encoding with value -1
    head->sound = -1;
    head->next = NULL;
}

int is_sound_list_empty(sound_node * head) {
    // Checks whether the first sound encoding has the value of -1
    return head->sound == -1;
}

void push_sound_list(sound_node ** head, int val) {
    if (is_sound_list_empty((*head))) {
        (*head) -> sound = val;
        return;
    }

    // Insert the given sound encoding to the beginning of the list
    sound_node * new_head = (sound_node *) malloc(sizeof(sound_node));
    new_head -> sound = val;
    new_head -> next = *head;
    (*head) = new_head;
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
        // Pop the first sound encoding in the list and change the pointer of the list to the next element
        sound_node * new_head = (*head) -> next;
        int sound = (*head) -> sound;
        free(*head);
        *head = new_head;
        return sound;
    }
}
