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
    int id;
    status status;
} Robot;

int can_free(void);
int can_work(void);
int get_my_id(void);
void go_done(void);
void go_free(void);
void go_work(void);
void init_robots(void);
void new_robot(int id);
void set_my_id(int id);

#ifdef __cplusplus
}
#endif

#endif
