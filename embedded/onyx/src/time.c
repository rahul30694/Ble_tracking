#include "time.h"
	
#include <stdio.h>
#include <stdint.h>

/*
#define CLOCKS_PER_CPU_FREQ (CLOCKS_PER_SEC / CPU_FREQ)

unsigned long getTicks()
{
	return (unsigned long)(clock() / CLOCKS_PER_CPU_FREQ);
}

unsigned long convertTicksToSec(unsigned long ticks)
{
	return (unsigned long)(ticks / CPU_FREQ);
}

unsigned long convertSecToTicks(unsigned long sec)
{
	return (unsigned long)(sec * CPU_FREQ);
}

unsigned long convertTicksToMilliSec(unsigned long ticks)
{
	return (unsigned long)((double(ticks) * 1000) / CPU_FREQ);
}

unsigned long convertTicksToMicroSec(unsigned long ticks)
{
	return (unsigned long)((double(ticks) * 1000000) / CPU_FREQ);
}


unsigned long millis()
{
	return (convertTicksToMilliSec(getTicks()));

}

unsigned long micros()
{
	return (convertTicksToMicroSec(getTicks()));

}

void delayMicroseconds(unsigned int us)
{
	unsigned long start = getTicks();
	while (convertTicksToMicroSec(getTicks() - start) < us);
}

void delay(unsigned long ms)
{
	unsigned long start = getTicks();
	while (convertTicksToMilliSec(getTicks() - start) < ms);
}

*/
