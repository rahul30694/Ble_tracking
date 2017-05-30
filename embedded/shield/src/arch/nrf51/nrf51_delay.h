#ifndef _NRF51_DELAY_H_
#define _NRF51_DELAY_H_

#include "nrf.h"

#ifdef __cplusplus
extern "C" {
#endif

void nrf51_delay_ms(uint32_t volatile ms);

static void __INLINE nrf51_delay_us(uint32_t volatile us) __attribute__((always_inline));
static void __INLINE nrf51_delay_us(uint32_t volatile us)
{
    register uint32_t delay __ASM ("r0") = us;
    __ASM volatile (

        ".syntax unified\n"

        "1:\n"
        " SUBS %0, %0, #1\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"   
        " NOP\n"  
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"
        " NOP\n"

        " BNE 1b\n"

        ".syntax divided\n"
        : "+r" (delay));
}

#ifdef __cplusplus
}
#endif

#endif
