#ifndef _CIRCULARBUFFER_H_
#define _CIRCULARBUFFER_H_

#include "queue.h"

typedef queue_t circBuff_t;

#define CIRCBUFF_DEF(n,t,s) QUEUE_DEF(n,t,s)

#endif