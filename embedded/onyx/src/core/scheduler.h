#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <pthread.h>
#include <stdint.h>

#define MAX_NUM_TASKS							20


#define sizeInBytes(x) 						((x >> 3) + 1)
#define task_init(t,p,a,l) 					task_create(t,#t,p,a,l)


typedef void *(*DART_TASK_T)(void *);
typedef void (*DART_CALLBACK_T)(int);

struct taskcontrolblock_s {
	const char *name;

 	DART_TASK_T routine;
 	DART_CALLBACK_T cleanup_callback;

 	void *arg;
 	unsigned arg_len;

 	union {
 		uint8_t flags;
 		struct {
 			//uint8_t created:1;
 			//uint8_t running:1;
 			//uint8_t blocked:1;
 			//uint8_t suspended:1;
 			//uint8_t killed:1;
 			//uint8_t completed:1;
 			//uint8_t;
 		}; 
 	};

 	int priority; 

 	pthread_t thread;
}__attribute__((packed));


int get_self_tid(void);

int task_create(DART_TASK_T routine, const char *const name, int priority, void *arg, unsigned arg_len);
int task_delete(int tid);

int scheduler_init(void);
void scheduler_run(void);
void scheduler_stop(void);


#endif