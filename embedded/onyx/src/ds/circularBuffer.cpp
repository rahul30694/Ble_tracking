#include <stdint.h>
#include <iostream>
#include <stdio.h>

#include "circularBuffer.h"


#ifdef _CIRCULAR_BUFFER_TEST_

CIRCBUF_DEF(int, abhi, 20);

int main()
{
	int a=5;
	abhi.enque(&a);
	a++;
	abhi.enque(&a);
	a++;
	abhi.enque(&a);
	a++;
	abhi.enque(&a);
	a++;
	abhi.enque(&a);
	a++;
	abhi.enque(&a);
	a++;
	abhi.enque(&a);

	return 0;
}
#endif

