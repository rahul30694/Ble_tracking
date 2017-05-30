#include "semaphore.h"

#include <semaphore.h>

/*
 * Initializing a semaphore that has already been initialized results in undefined behavior.
 */
int semaphore_init(semaphore_t *sem, unsigned value)
{
	if (!sem)
		return -1;
	if (sem_init(&sem->s, 0, value))
		return -1;
	return 0;
}

/*
 * Destroying a semaphore that other processes or threads are currently blocked on produces undefined behavior.
 * Using a semaphore that has been destroyed produces undefined results, until the semaphore has been reinitialized.
 *
 */
int semaphore_destroy(semaphore_t *sem)
{
	if (!sem)
		return -1;
	if (sem_destroy(&sem->s))
		return -1;
	return 0;
}


/*
 * sem_post() is async-signal-safe: it may be safely called within a signal handler.
 */
int semaphore_post(semaphore_t *sem)
{
	if (!sem)
		return -1;
	if (sem_post(&sem->s))
		return -1;
	return 0;
}


int semaphore_wait(semaphore_t *sem)
{
	if (!sem)
		return -1;
	if (sem_wait(&sem->s))
		return -1;
	return 0;
}

int semaphore_trywait(semaphore_t *sem)
{
	if (!sem)
		return -1;
	if (sem_trywait(&sem->s))
		return -1;
	return 0;
}

