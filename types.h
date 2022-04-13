#ifndef TYPES_H
#define TYPES_H

/**
 * A coordinate type consists of position x and y
 */
typedef struct {
    int x;
    int y;
} Coordinate;

/**
 * Checks whether 2 coordinates are equal
 * @param c1 Coordinate 1
 * @param c2 Coordinate 2
 * @return 1 if both coordinates are the same, 0 otherwise
 */
int compare_coordinate(Coordinate * c1, Coordinate * c2);

/**
 * Print a given coordinate in tuple form
 * @param c Coordinate to be printed
 */
void print_coordinate(Coordinate * c);

/**
 * A linked list of coordinates
 */
typedef struct c_node {
    Coordinate val;
    struct c_node * next;
} coordinate_node;

/**
 * Create a new empty coordinate list
 * @param head A pointer to the list
 */
void new_coordinate_list(coordinate_node * head);

/**
 * Copy all the elements of coordinate list into a new pointer
 * @param head A coordinate list to be copied
 * @return A pointer to a new equal coordinate list
 */
coordinate_node * copy_coordinate_list(coordinate_node * head);

/**
 * Print a given coordinate list
 * @param head Coordinate list to be printed
 */
void print_coordinate_list(coordinate_node * head);

/**
 * Get the length of the coordinate list
 * @param head A pointer to a coordinate list
 * @return The length of the given coordinate list
 */
int coordinate_list_length(coordinate_node * head);

/**
 * Get the last coordinate from a given coordinate list
 * @param head A pointer to the coordinate list
 * @return The last coordinate in the given coordinate list
 */
Coordinate * coordinate_list_last(coordinate_node * head);

/**
 * Checks whether a given coordinate list is empty
 * @param head A pointer to the coordinate list
 * @return 1 if the given coordinate list is empty, 0 otherwise
 */
int is_coordinate_list_empty(coordinate_node * head);

/**
 * Checks whether a coordinate list contains a given coordinate
 * @param head A pointer to the coordinate list
 * @param c Coordinate to be checked
 * @return 1 if the given coordinate list contains the given coordinate, 0 otherwise
 */
int coordinate_list_contains(coordinate_node * head, Coordinate * c);

/**
 * Add a coordinate to the given coordinate list
 * @param head A pointer to the coordinate list
 * @param c Coordinate to be pushed
 */
void push_coordinate_list(coordinate_node * head, Coordinate c);

/**
 * List of paths (sequence of coordinates)
 */
typedef struct p_node {
    coordinate_node * val;
    struct p_node * next;
} path_node;

/**
 * Create a new path list
 * @param head A pointer to the path list
 */
void new_path_list(path_node * head);

/**
 * Prints the path list
 * @param head A path list to be printed
 */
void print_path_list(path_node * head);

/**
 * Checks whether a given path list is empty
 * @param head A pointer to a path list
 * @return 1 if the given path list is empty, 0 otherwise
 */
int is_path_list_empty(path_node * head);

/**
 * Push a path to the path list
 * @param head A pointer to a path list
 * @param val Path to be pushed
 */
void push_path_list(path_node * head, coordinate_node * val);

/**
 * Direction, where the robot is currently facing
 */
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} direction;

/**
 * A robot consists of ID, coordinate, and direction
 */
typedef struct {
    int id;
    Coordinate * coordinate;
    direction direction;
} Robot;

/**
 * A list of robots
 */
typedef struct r_node {
    Robot * robot;
    struct r_node * next;
} robot_node;

/**
 * Create a new robot list
 * @param head A pointer to a robot list
 */
void new_robot_list(robot_node * head);

/**
 * Print a given robot list
 * @param head A pointer to a robot list
 */
void print_robot_list(robot_node * head);

/**
 * Checks whether a given robot list is empty
 * @param head A robot list to be checked
 * @return 1 if the given robot list is empty, 0 otherwise
 */
int is_robot_list_empty(robot_node * head);

/**
 * Push a robot to the end of a given robot list
 * @param head A pointer to a robot list
 * @param val A robot to be pushed
 */
void push_robot_list(robot_node * head, Robot * val);

/**
 * Pop a robot from the beginning of a given robot list
 * @param head A pointer to a robot list
 * @return The first robot of the list
 */
Robot * pop_robot_list(robot_node ** head);

/**
 * A linked list for storing sound encoding
 */
typedef struct s_node {
    int sound;
    struct s_node * next;
} sound_node;

/**
 * Crate a new sound list
 * @param head A pointer to a sound list
 */
void new_sound_list(sound_node *head);

/**
 * Checks whether a given sound list is empty
 * @param head A sound list to be checked
 * @return 1 if the given sound list is empty, 0 otherwise
 */
int is_sound_list_empty(sound_node * head);

/**
 * Push a sound encoding into the beginning of the given sound list
 * @param head A pointer to a sound list
 * @param val A sound encoding to be pushed
 */
void push_sound_list(sound_node ** head, int val);

/**
 * Pop a sound encoding from the beginning of the given sound list
 * @param head A pointer to a sound list
 * @return The first sound encoding in the list
 */
int pop_sound_list(sound_node ** head);

#endif //TYPES_H
