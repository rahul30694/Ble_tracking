#include "nrf51/nrf51_delay.h"

void delay_ms(uint32_t ms)
{
	nrf51_delay_ms(ms);
}

void delay_us(uint32_t us)
{
	nrf51_delay_us(us);
}
