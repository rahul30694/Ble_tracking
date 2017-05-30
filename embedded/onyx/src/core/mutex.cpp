#include "mutex.h"

#include <pthread.h>

/*
 * PTHREAD_MUTEX_NORMAL: This type of mutex does not detect deadlock. 
 * A thread attempting to relock this mutex without first unlocking it shall deadlock. 
 * Attempting to unlock a mutex locked by a different thread results in undefined behavior. 
 * Attempting to unlock an unlocked mutex results in undefined behavior.
 *
 * PTHREAD_PROCESS_PRIVATE: The mutex shall only be operated upon by threads created within the same process 
 * as the thread that initialized the mutex; if threads of differing processes attempt to operate on such a mutex, 
 * the behavior is undefined.
 *
 * PTHREAD_PRIO_NONE: When a thread owns a mutex with the  protocol attribute, 
 * its priority and scheduling shall not be affected by its mutex ownership.
 *
 * Only mutex itself may be used for performing synchronization. The result of referring to copies of mutex 
 * in calls to pthread_mutex_lock(), pthread_mutex_trylock(), pthread_mutex_unlock(), and pthread_mutex_destroy() is undefined.
 */

/*
 * It is better to be specific to increase portability across various different platforms and implementations.
 * Attempting to initialize an already initialized mutex results in undefined behavior.
 */

#define CAST_TO_PMUTEX(x)	(x)

int mutex_init(mutex_t *m)
{
	pthread_mutexattr_t attr;

	if (pthread_mutexattr_init(&attr))
		return -1;
	if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL))
		return -1;
	if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE))
		return -1;
	if (pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_NONE))
		return -1;

	if (pthread_mutex_init(CAST_TO_PMUTEX(&(m->mutex)), &attr))
		return -1;
	return 0;
}

int mutex_lock(mutex_t *m)
{
	if (pthread_mutex_lock(CAST_TO_PMUTEX(&(m->mutex))))
		return -1;
	return 0;
}

int mutex_unlock(mutex_t *m)
{
	if (pthread_mutex_unlock(CAST_TO_PMUTEX(&(m->mutex))))
		return -1;
	return 0;
}

int mutex_trylock(mutex_t *m)
{
	if (pthread_mutex_trylock(CAST_TO_PMUTEX(&(m->mutex))))
		return -1;
	return 0;
}

/* 
 * It shall be safe to destroy an initialized mutex that is unlocked. 
 * Attempting to destroy a locked mutex results in undefined behavior.
 */
int mutex_destroy(mutex_t *m)
{
	if (pthread_mutex_destroy(CAST_TO_PMUTEX(&(m->mutex))))
		return -1;
	return 0;
}

#define __MAIN__

#ifdef __MAIN__
int main()
{
	return 0;
}
#endif
