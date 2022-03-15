#include "types.h"

// Coordinate
int compare_coordinate(Coordinate * c1, Coordinate * c2) {
    return c1->x == c2->x && c1->y == c2->y;
}

void print_coordinate(Coordinate * c) {
    printf("(%d, %d)", c->x, c->y);
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
    return head->val.x == -1 && head->val.y == -1 && head->next == NULL;
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
    c->x = -1;
    c->y = -1;
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

// Path list
int is_path_list_empty(path_node * head) {
    return head->val->val.x == -1 && head->val->val.y == -1 && head->next == NULL;
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