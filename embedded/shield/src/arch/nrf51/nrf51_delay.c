#include "nrf51_delay.h"

#include "nrf.h"


void nrf51_delay_ms(uint32_t volatile ms)
{
    while (ms != 0) {
        ms--;
        nrf51_delay_us(999);
    }
}
