#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "circularBuffer.h"


//CIRCBUFF_DEF(circ, int, 20);


int8_t genericCircBuffEnque(circBuff_t *circ, void *enqueData)
{
	uint16_t front = (circ->front + 1) % circ->queueSize;

	if(front == circ->rear) 
		return -1;
	if(enqueData == NULL)
		return -1;
	
	memcpy(circ->data + circ->front * circ->elementSize,
		enqueData, circ->elementSize);

	front = circ->front;
	return 0;
}

int8_t genericCircBufDeque(circBuff_t *circ, void *dequeData)
{
	if(circ->front == circ->rear)
		return -1;

	if(dequeData == NULL)
		return -1;

	memcpy(dequeData, circ->data + circ->rear * circ->elementSize, 
		circ->elementSize);
	circ->rear = (circ->rear + 1) % circ->queueSize;
	
	return 0;
}

int circBufGetFreeSpace(circBuff_t *circ) 
{
	int freeSpace;
	freeSpace = circ->rear - circ->front;
	if (freeSpace <= 0)
		freeSpace += circ->queueSize;
	return freeSpace;
}

void resetCircBuf(circBuff_t *circ)
{
	circ->front = circ->rear = 0;
}

bool isCircBuffEmpty(circBuff_t *circ)
{
	if(circ->front == circ->rear)
		return true;
	
	return false;
}

/*
int main()
{
	return 0;
}
*/
