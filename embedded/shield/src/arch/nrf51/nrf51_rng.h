#ifndef _NRF51_RNG_H_
#define _NRF51_RNG_H_

#include "nrf.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum nrf51_rng_task_e {
	NRF51_RNG_TASK_START	=	offsetof(NRF_RNG_Type, TASKS_START),
	NRF51_RNG_TASK_STOP 	=	offsetof(NRF_RNG_Type, TASKS_STOP) 
};

enum nrf51_rng_event_e {
	NRF51_RNG_EVENT_VALRDY	=	offsetof(NRF_RNG_Type, EVENTS_VALRDY)
};

enum nrf51_rng_int_mask_e {
	NRF51_RNG_INT_VALRDY_MASK	=	RNG_INTENSET_VALRDY_Msk,
};

enum nrf51_rng_short_mask_e {
	NRF51_RNG_SHORT_VALRDY_STOP_MASK = RNG_SHORTS_VALRDY_STOP_Msk,
};

void nrf51_rng_start();
void nrf51_rng_stop();
uint8_t nrf51_rng_read(void);
void nrf51_rng_task_trigger(enum nrf51_rng_task_e task);
bool nrf51_rng_event_read(enum nrf51_rng_event_e event);
void nrf51_rng_event_clear(enum nrf51_rng_event_e event);
void nrf51_rng_int_enable(uint32_t mask);
void nrf51_rng_int_disable(uint32_t mask);
void nrf51_rng_error_correction_enable(void);
void nrf51_rng_error_correction_disable(void);
uint32_t *nrf51_rng_task_address_get(enum nrf51_rng_task_e task);
uint32_t *nrf_rng_event_address_get(enum nrf51_rng_event_e event);
void nrf51_rng_shorts_enable(uint32_t mask);
void nrf51_rng_shorts_disable(uint32_t mask);


#ifdef __cplusplus
}
#endif

#endif