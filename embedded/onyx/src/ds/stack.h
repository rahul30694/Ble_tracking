/*
 * stack.h
 *
 *  Created on: 03-Apr-2016
 *      Author: abhiarora
 */

#ifndef _STACK_H_
#define _STACK_H_

template <typename T>
class Stack {
private:
	T *const data;
	unsigned int top;
	unsigned int length;

protected:

public:
	Stack();
	Stack(T *const buf, unsigned int len);

	int push(T *p);
	int pop(T *p, bool readOnly);
	int pop(T *p) { return pop(p,false); }

	int len() { return top; }
	int freeSpace() {return (length - top);}
};



#endif /* HELPER_STACK_H_ */
