#include "nrf51_temp.h"

#define NRF51_TEMP_TASK_TRIGGER 		(1UL)
#define NRF51_TEMP_EVENT_CLEAR  		(0UL)

#define MASK_SIGN           			(0x00000200UL)
#define MASK_SIGN_EXTENSION 			(0xFFFFFC00UL)

void nrf51_temp_task_trigger(enum nrf51_temp_task_e task)
{
	*(volatile uint32_t *)((uint8_t *)NRF_TEMP + task) = NRF51_TEMP_TASK_TRIGGER;
}

bool nrf51_temp_event_read(enum nrf51_temp_event_e event)
{
	return !!(*(volatile uint32_t *)((uint8_t *)NRF_TEMP + event));
}

void nrf51_temp_event_clear(enum nrf51_temp_event_e event)
{
	*(volatile uint32_t *)((uint8_t *)NRF_TEMP + event) = NRF51_TEMP_EVENT_CLEAR;
}

void nrf51_temp_int_enable(uint32_t mask)
{
	NRF_TEMP->INTENSET = mask;
}

void nrf51_temp_int_disable(uint32_t mask)
{
	NRF_TEMP->INTENCLR = mask;
}

void nrf51_temp_init(void)
{
    /* Workaround for PAN_028 rev2.0A anomaly 31 - TEMP: Temperature offset value has to be manually loaded to the TEMP module */
    *(uint32_t *) 0x4000C504 = 0;
}

int32_t nrf51_temp_read(void)
{    
    /* Workaround for PAN_028 rev2.0A anomaly 28 - TEMP: Negative measured values are not represented correctly */
    return ((NRF_TEMP->TEMP & MASK_SIGN) != 0) ? (NRF_TEMP->TEMP | MASK_SIGN_EXTENSION) : (NRF_TEMP->TEMP);    
}