#include "scheduler.h"

#include "../include/system/systemlogline.h"

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <errno.h>

// Non C-Standard Library
// POSIX Standard
#include <pthread.h>
#include <unistd.h>

/*---------- READ SECTION -----------------*/
// Tasks in this DARTOS implementation are being implemented
// using PThreads in C.

// https://www.cs.cf.ac.uk/Dave/C/node30.html
// https://users.cs.cf.ac.uk/Dave.Marshall/C/node29.html
// http://stackoverflow.com/questions/7635515/how-to-set-cap-sys-nice-capability-to-a-linux-user/37528755#37528755
// http://man7.org/linux/man-pages/man7/sched.7.html
/*---------- END --------------------------*/

#define MAX_DARTOS_THREADS					MAX_NUM_TASKS

#define max(x,y)							((x)>(y) ? (x) : (y))

static struct taskcontrolblock_s sysTask[MAX_DARTOS_THREADS];
static unsigned max_task_id_registered = 0;

static std :: bitset<sizeInBytes(MAX_DARTOS_THREADS)> taskIDs;


static int assignNewTID()
{
	int i;
	for (i=0; i<taskIDs.size(); i++) {
		if (!taskIDs[i])
			goto ASSIGN_TID;
	}
	return -1;

ASSIGN_TID:
	max_task_id_registered = max(max_task_id_registered, i);
	taskIDs[i] = 1;
	return i;
}


int get_self_tid()
{
	pthread_t p = pthread_self();

	int i;
	for (i=0; i<=max_task_id_registered; i++) {
		if (taskIDs[i]) {
			if (pthread_equal(sysTask[i].thread, p))
				return i;
		}
	}
	return -1;
}

int task_create(DART_TASK_T routine, const char *const name, int priority, void *arg, unsigned arg_len)
{
	if ((arg) and (arg_len == 0))
		return -1;

	int tid = assignNewTID();
	if (tid < 0)
		return -1;

	sysTask[tid].routine = routine;
	sysTask[tid].name = name;

	sysTask[tid].arg = arg;
	sysTask[tid].arg_len = arg_len;
	sysTask[tid].priority = priority;

	sysTask[tid].flags = 0x00;
	//sysTask[tid].created = 1;
	return tid;
}

/*
 * If the calling task is the only task in the highest priority list
 * at that time, it will continue to run after a call to task_yield().
 */

int task_yield()
{
	if (sched_yield())
		return -1;
	return 0;
}

void task_register_cleanup_callback(void (*cleanup_callback)(int))
{
	int tid = get_self_tid();
	if (tid < 0)
		return;
	sysTask[tid].cleanup_callback = cleanup_callback;
	return;
}

void task_exit(int retval)
{
	int tid = get_self_tid();
	if (tid < 0)
		return;

	if (sysTask[tid].cleanup_callback)
		sysTask[tid].cleanup_callback(retval);
	pthread_exit(&retval);
}

int task_delete(int tid)
{
	if (tid >= MAX_DARTOS_THREADS)
		return -1;
	if (!taskIDs[tid])
		return -1;

	if (pthread_cancel(sysTask[tid].thread))
		return -1;
	if (sysTask[tid].cleanup_callback)
		sysTask[tid].cleanup_callback(-2);

	taskIDs[tid] = 0;
	return 0;
}

static int dispatch_task(int tid)
{
	if (!taskIDs[tid])
		return -1;

	pthread_attr_t attr;
	struct sched_param param;
	param.sched_priority = sysTask[tid].priority;
	int policy = SCHED_RR;

	if (pthread_attr_init(&attr))
		return -1;
	if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED))
		goto FAILED;
	if (pthread_attr_setschedpolicy(&attr, policy))
		goto FAILED;
	if (pthread_attr_setschedparam(&attr, &param))
		goto FAILED;
	if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM))
		goto FAILED;
//#define DEFAULT_PTHREAD_STACK_SIZE				0
	//if (pthread_attr_setstacksize(&attr, DEFAULT_PTHREAD_STACK_SIZE))
		//return -1;
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
		goto FAILED;
	if (errno = pthread_create(&sysTask[tid].thread, &attr, sysTask[tid].routine, sysTask[tid].arg))
		goto FAILED;


	pthread_attr_destroy(&attr);
	return 0;

FAILED:
	pthread_attr_destroy(&attr);
	return -1;
}


int scheduler_init()
{
	memset(sysTask, 0, sizeof(sysTask));
	taskIDs.reset();
	return 0;
}

void scheduler_run()
{
	int i;
	for (i=0; i < MAX_DARTOS_THREADS; i++) {
		if (taskIDs[i]) {
			if (dispatch_task(i))
				LOGLINE_CRITICAL("Failed to dispath Task: %s\n", sysTask[i].name);
			else
				LOGLINE_INFO("Task Started: %s\n", sysTask[i].name);
		}
	}
}

void scheduler_stop()
{
	int i;
	for (i=0; i < MAX_DARTOS_THREADS; i++) {
		if (taskIDs[i]) {
			if (task_delete(i)) {
				//LOGLINE_CRITICAL("Failed to Delete Task: %s\n", sysTask[i].name);
			} else {
				//LOGLINE_INFO("Task Deleted: %s\n", sysTask[i].name);
			}
		}
	}
	// exit(0);
}

void task_first_cleanup(int retval)
{
	printf("Thread First Finished\n");
}

void task_second_cleanup(int retval)
{
	printf("Thread Second Finished\n");
}


void *task_first(void *arg)
{
	task_register_cleanup_callback(task_first_cleanup);
	int *i = static_cast<int *>(arg);
	while (1) {
		printf("Hello First Task(%d) - %d\n", get_self_tid(), *i);
		(*i)++;
		//task_yield();
	}
}


void *task_second(void *arg)
{
	task_register_cleanup_callback(task_second_cleanup);
	int *i = static_cast<int *>(arg);
	while (1) {
		printf("Hello Second Task(%d) - %d\n", get_self_tid(), *i);
		(*i)++;
		//task_yield();
	}
}

#define __MAIN__

#ifdef __MAIN__
int main()
{
	// init_logline("onyx");
	scheduler_init();

	int task_arg_1 = 0;
	int task_arg_2 = 0;
	task_init(task_first, 1, &task_arg_1, sizeof(task_arg_1));
	task_init(task_second, 1, &task_arg_2, sizeof(task_arg_1));
	scheduler_run();


	while (1) {
		printf("Main Task\n");
		usleep(100);
		break;
	}
	scheduler_stop();
	while (1);
	return 0;
}
#endif