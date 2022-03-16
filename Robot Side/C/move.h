#ifndef MOVE_H
#define MOVE_H

#ifdef __cplusplus
extern "C" {
#endif


double find_speed(int speed);
double calculateF(int speed);
double calculateVrobot(int speed);
double calculateS(double angle);
double calculateTime(double angle, int speed);
void turnRight(int speed);
void turnLeft(int speed);
void moveForward(int speed);
void moveBackward(int speed);
void stop(void);
void parseMovement(char* movementSet, int len, int speed);

#ifdef __cplusplus
}
#endif

#endif
