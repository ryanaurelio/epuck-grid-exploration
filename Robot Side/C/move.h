#ifndef MOVE_H
#define MOVE_H

#ifdef __cplusplus
extern "C" {
#endif


void turnRight(int speed);
void turnLeft(int speed);
void moveForward(int speed);
void moveBackward(int speed);
void stop(void);

#ifdef __cplusplus
}
#endif

#endif
