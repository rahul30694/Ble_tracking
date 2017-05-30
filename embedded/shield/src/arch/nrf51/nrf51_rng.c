#include "nrf51_rng.h"

#define NRF51_RNG_TASK_TRIGGER 		(1UL)
#define NRF51_RNG_EVENT_CLEAR  		(0UL)



void nrf51_rng_start()
{
	nrf51_rng_task_trigger(NRF51_RNG_TASK_START);
}

void nrf51_rng_stop()
{
	nrf51_rng_task_trigger(NRF51_RNG_TASK_STOP);
}

uint8_t nrf51_rng_read(void)
{
	return (NRF_RNG->VALUE & RNG_VALUE_VALUE_Msk);
}

void nrf51_rng_task_trigger(enum nrf51_rng_task_e task)
{
	*(volatile uint32_t *)((uint8_t *)NRF_RNG + task) = NRF51_RNG_TASK_TRIGGER;
}

bool nrf51_rng_event_read(enum nrf51_rng_event_e event)
{
	return !!(*(volatile uint32_t *)((uint8_t *)NRF_RNG + event));
}

void nrf51_rng_event_clear(enum nrf51_rng_event_e event)
{
	*((volatile uint32_t *)((uint8_t *)NRF_RNG + event)) = NRF51_RNG_EVENT_CLEAR;
}

void nrf51_rng_int_enable(uint32_t mask)
{
	NRF_RNG->INTENSET = mask;
}

void nrf51_rng_int_disable(uint32_t mask)
{
	NRF_RNG->INTENCLR = mask;
}


void nrf51_rng_error_correction_enable(void)
{
    NRF_RNG->CONFIG |= RNG_CONFIG_DERCEN_Msk;
}

void nrf51_rng_error_correction_disable(void)
{
    NRF_RNG->CONFIG &= ~RNG_CONFIG_DERCEN_Msk;
}

uint32_t *nrf51_rng_task_address_get(enum nrf51_rng_task_e task)
{
    return (uint32_t *)((uint8_t *)NRF_RNG + task);
}

uint32_t *nrf_rng_event_address_get(enum nrf51_rng_event_e event)
{
    return (uint32_t *)((uint8_t *)NRF_RNG + event);
}

 void nrf51_rng_shorts_enable(uint32_t mask)
{
     NRF_RNG->SHORTS |= mask;
}

void nrf51_rng_shorts_disable(uint32_t mask)
{
     NRF_RNG->SHORTS &= ~mask;
}
