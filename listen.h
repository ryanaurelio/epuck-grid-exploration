#ifndef LISTEN_H
#define LISTEN_H

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
 * start a thread on listen
 */
void listen_start(void);

/*
 * stop a thread on listen
 */
void listen_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* LISTEN_H */
