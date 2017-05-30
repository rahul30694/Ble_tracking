#include "stack.h"


template <typename T>
Stack<T>:: Stack(T *const buf, unsigned int len)
{
	data = buf;
	length = len;
	top = 0;
}

template <typename T>
int Stack<T>::push(T *const elem)
{
	if (!elem)
		return -1;

	if (top == length)
		return -1;

	memcpy(data + top, elem, sizeof(T));
	top++;
	return 0;
}

template <typename T>
int Stack<T>::pop(T *const elem, bool readOnly)
{
	if (!top)
		return -1;
	if (elem)
		memcpy(elem, data + (top - 1), sizeof(T));
	if (readOnly)
		top--;
	return 0;
}


