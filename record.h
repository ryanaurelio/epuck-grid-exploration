#ifndef RECORD_H
#define RECORD_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * start a thread on recording
 */
void record_start(void);

/*
 * kill a thread on recording
 */
void record_stop(void);

/*
 * call this to get the microphone value
 */
int listen(void);

#ifdef __cplusplus
}
#endif

#endif /* RECORD_H */
