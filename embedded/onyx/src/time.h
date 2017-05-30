#ifndef _TIME_H_
#define _TIME_H_

//#include <time.h>
#include <stdint.h>
/*
unsigned long getTicks();

unsigned long convertTicksToSec(unsigned long ticks);
unsigned long convertSecToTicks(unsigned long sec);
unsigned long convertTicksToMilliSec(unsigned long ticks);
unsigned long convertTicksToMicroSec(unsigned long ticks);

unsigned long millis();
unsigned long micros();

void delayMicroseconds(unsigned int us);
void delay(unsigned long ms);

*/
#define CPU_FREQ 1000000UL

#define scaleDownToFreq(f) { \
	static clock_t _tick; \
	if ((clock() - _tick) <  (CLOCKS_PER_SEC / f)) \
		continue; \
	_tick=clock(); \
	}

#endif
