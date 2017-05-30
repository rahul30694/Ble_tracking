/*
 * circularBuffer.h
 *
 *  Created on: 22-Feb-2016
 *      Author: abhiarora
 */

#ifndef _CIRCULARBUFFER_H_
#define _CIRCULARBUFFER_H_

#include "queue.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CIRCBUF_DEF(t,n,s) t __buf_##n[s]; \
	circularBuffer<t> n = circularBuffer<t>(__buf_##n, s);

template <class _elem_t>
class circularBuffer : public queue<_elem_t> {

public:
	circularBuffer(_elem_t *buf, int len) : queue<_elem_t> :: queue(buf, len) { };

	int enque(_elem_t *data) { return enque(data, false); }
	int deque(_elem_t *data) { return deque(data, false); }


	int enque(_elem_t *data, bool force);
	int deque(_elem_t *data, bool peek);

	//Inline Functions!
	int peek(_elem_t *data) { return deque(data, true); }
	int enqueForce(_elem_t data) { return enque(data, true); }

	bool isEmpty(void);
	void reset(void);
	size_t getUsedSpace(void);
	size_t getFreeSpace(void);

};

template <class _elem_t>
int circularBuffer<_elem_t> :: enque(_elem_t *enq, bool force)
{
	if (!enq)
		return -1;

	uint16_t next = (this->front + 1) % this->qLen;
	if (next == this->rear) {
		if (force)
			this->rear = (this->rear + 1) % this->qLen;
		else
			return -1;
	}

	memcpy(this->data + this->front, enq, sizeof(_elem_t));
	this->front = next;
	return 0;
}

template <class _elem_t>
int circularBuffer<_elem_t> :: deque(_elem_t *deq, bool peak)
{
	if (this->front == this->rear)
		return -1;

	if (deq)
		memcpy(deq, this->data + this->rear, sizeof(_elem_t));
	if (!peak)
		this->rear = (this->rear + 1) % this->qLen;
	return 0;
}

template <class _elem_t>
size_t circularBuffer<_elem_t> :: getUsedSpace()
{
	int usedSpace;
	usedSpace = this->front - this->rear;
	if (usedSpace < 0)
		usedSpace += this->qLen;
	return usedSpace;
}

template <class _elem_t>
size_t circularBuffer<_elem_t> :: getFreeSpace()
{
	int freeSpace;
	freeSpace = this->rear - this->front;
	if (freeSpace <= 0)
		freeSpace += this->qLen;
	return freeSpace;
}

template <class _elem_t>
bool circularBuffer<_elem_t> :: isEmpty()
{
	if (this->front == this->rear)
		return true;
	return false;
}

template <class _elem_t>
void circularBuffer<_elem_t> :: reset()
{
	this->front = this->rear = 0;
}


#endif /* _CIRCULARBUFFER_H_ */
