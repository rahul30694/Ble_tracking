#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

void timer_init(void);
unsigned long millis(void);

unsigned long micros(void);

#define CONVERT_TO_PRESCALER(x) ((32768/(x)) - 1)

#define systick_init(freq,handler)	systick_init_prescaler(CONVERT_TO_PRESCALER(freq),handler)

void systick_init_prescaler(uint32_t val, void (*event_handler)(void));
unsigned long systick(void);

#endif
