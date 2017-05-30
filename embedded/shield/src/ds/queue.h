#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stddef.h>
#include <string.h>

#define QUEUE_DEF(t,n,s) t __buf_##n[s]; \
	queue n = queue(__buf_##n, s);


template <class _elem_t>
class queue {
protected:
	_elem_t *data;
	int front;
	int rear;
	int qLen;

public:
	queue() { data=NULL; front=0; rear=0; qLen=0; }
	queue(_elem_t *data, int len);

	virtual int enque(_elem_t *data);
	virtual int deque(_elem_t *data);
	virtual bool isEmpty();
	virtual size_t getUsedSpace();
	virtual size_t getFreeSpace();

};

template <class _elem_t>
queue<_elem_t>::queue(_elem_t *buf, int len)
{
	data = buf;
	front = rear = 0;
	qLen = len;
}

#include <stddef.h>



template <class _elem_t>
int queue<_elem_t>::enque(_elem_t *enq)
{
	if (front == (qLen - 1))
		return -1;

	if (!enq)
		return -1;

	memcpy(data + front, enq, sizeof(_elem_t));

	front++;
	return 0;
}

template <class _elem_t>
int queue<_elem_t>::deque(_elem_t *deqData)
{
	if (front == rear) {
		front = rear = 0;
		return -1;
	}

	if (!deqData)
		return -1;

	memcpy(deqData, data + rear, sizeof(_elem_t));
	rear++;
	return 0;
}

template <class _elem_t>
bool queue<_elem_t>::isEmpty()
{
	if (front != rear)
		return false;

	front = rear = 0;
	return true;
}

template <class _elem_t>
size_t queue<_elem_t>::getUsedSpace()
{
	return (front - rear);

}

template <class _elem_t>
size_t queue<_elem_t>::getFreeSpace()
{
	return (qLen - front);
}


#endif
