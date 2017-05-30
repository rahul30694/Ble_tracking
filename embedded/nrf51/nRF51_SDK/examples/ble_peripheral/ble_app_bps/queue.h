#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	void * const data;
	uint16_t front;
	uint16_t rear;
	const uint16_t queueSize;
	const uint16_t elementSize;
}queue_t;

#define QUEUE_DEF(n,t,s) t buffer_##n[s]; \
	queue_t n = {.data = (void *)buffer_##n, \
		.front=0, \
		.rear=0, \
		.elementSize = sizeof(t), \
		.queueSize = s }


int8_t genericEnque(queue_t *, void *);
int8_t genericDeque(queue_t *, void *);

bool isQueueEmpty(queue_t *);
int queueGetFreeSpace(queue_t *);


#endif