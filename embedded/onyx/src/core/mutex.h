#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <pthread.h>

typedef struct mutex_s {
	pthread_mutex_t mutex;
} mutex_t;

int mutex_init(mutex_t *m);

int mutex_lock(mutex_t *m);
int mutex_unlock(mutex_t *m);
int mutex_trylock(mutex_t *m);

int mutex_destroy(mutex_t *m);


#endif