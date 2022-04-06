#ifndef CONNECT_H
#define CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  start a thread on connection
 */
void connect_start(void);

/*
 * after connection finished, kill the thread to clean
 */
int connect_finished(void);

#ifdef __cplusplus
}
#endif

#endif /* CONNECT_H */
