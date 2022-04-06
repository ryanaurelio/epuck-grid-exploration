#ifndef WORK_H
#define WORK_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * broadcast guide
 * xxx 		| ooxxx | ooxxx | xx 		|
 * robot id | x 	| y 	| indicator |
 *
 * oo is a negative / positive / done indicator
 * 00 is positive
 * 01 is negative
 * 11 is done
 */


/*
 * start a thread on working
 */
void work_start(void);

/*
 * stop a thread on working
 */
void work_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* WORK_H */
