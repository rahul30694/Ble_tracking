#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <semaphore.h>

typedef struct {
	sem_t s;
} semaphore_t;

int semaphore_init(semaphore_t *sem, unsigned value);
int semaphore_destroy(semaphore_t *sem);

int semaphore_post(semaphore_t *sem);

int semaphore_wait(semaphore_t *sem);
int semaphore_trywait(semaphore_t *sem);



#endif