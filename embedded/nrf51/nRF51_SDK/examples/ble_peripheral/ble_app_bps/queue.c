#include "queue.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <stdbool.h>

QUEUE_DEF(queue, uint16_t, 20);

int8_t genericEnque(queue_t *que, void *pushData)
{
	if(que->front == que->queueSize) 
		return -1;
	if(pushData == NULL)
		return -1;
	
	memcpy(que->data + que->front * que->elementSize,
	 pushData, que->elementSize);

	que->front++;
	return 0;
}

int8_t genericDeque(queue_t *que, void *popData)
{
	if(que->front == que->rear) {
		que->front = que->rear = 0;
		return -1;
	}
	if(popData == NULL)
		return -1;

	memcpy(popData, que->data + que->rear * que->elementSize, que->elementSize);
	que->rear++;
	return 0;
}

bool isQueueEmpty(queue_t *que)
{
	if(que->front == que->rear) {
		que->front = que->rear = 0;

		return true;
	}
	return false;
}

int queueGetFreeSpace(queue_t *que)
{
	int freeSpace;
	freeSpace = que->front - que->rear;

	return freeSpace;
}
/*
int main()
{
	uint16_t i=0;
	for(i = 0; i <25; i++) {
		if(genericQueue(&queue, (void *)&i) != 0)
			printf("Error in queuing -%d \n", i);
	}
	uint16_t data;
	for(i = 0; i <22; i++) {
		if(genericDequeue(&queue, (void *)&data) == 0)
			printf("Data - %u \n", data);
		else
			printf("Error in reading - %d\n", i);
	}
	return 0;
}
*/