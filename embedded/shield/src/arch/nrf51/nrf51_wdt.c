#include "nrf51_wdt.h"

#define NRF51_WDT_CHANNEL_NUMBER 	0x8UL
#define NRF51_WDT_RR_VALUE       	0x6E524635UL /* Fixed value, shouldn't be modified.*/

#define NRF51_WDT_TASK_TRIGGER       	1UL
#define NRF51_WDT_EVENT_CLEAR    		0UL



void nrf51_wdt_behaviour_set(enum nrf51_wdt_behaviour_e behaviour)
{
    NRF_WDT->CONFIG = behaviour;
}

void nrf51_wdt_task_trigger(enum nrf51_wdt_task_e task)
{
    *((volatile uint32_t *)((uint8_t *)NRF_WDT + task)) = NRF51_WDT_TASK_TRIGGER;
}

void nrf51_wdt_event_clear(enum nrf51_wdt_event_e event)
{
    *((volatile uint32_t *)((uint8_t *)NRF_WDT + (uint32_t)event)) = NRF51_WDT_EVENT_CLEAR;
}

bool nrf51_wdt_event_check(enum nrf51_wdt_event_e event)
{
    return (bool)*((volatile uint32_t *)((uint8_t *)NRF_WDT + event));
}


void nrf51_wdt_int_enable(uint32_t mask)
{
    NRF_WDT->INTENSET = mask;
}


void nrf51_wdt_int_disable(uint32_t mask)
{
    NRF_WDT->INTENCLR = mask;
}

uint32_t nrf51_wdt_task_address_get(enum nrf51_wdt_task_e task)
{
    return ((uint32_t)NRF_WDT + task);
}

uint32_t nrf51_wdt_event_address_get(enum nrf51_wdt_event_e event)
{
    return ((uint32_t)NRF_WDT + event);
}

bool nrf51_wdt_started(void)
{
    return (bool)(NRF_WDT->RUNSTATUS);
}


bool nrf51_wdt_request_status(enum nrf51_wdt_rr_register_e rr_register)
{
    return (bool)(((NRF_WDT->REQSTATUS) >> rr_register) & 0x1UL);
}


void nrf51_wdt_reload_value_set(uint32_t reload_value)
{
    NRF_WDT->CRV = reload_value;
}


void nrf51_wdt_reload_request_enable(enum nrf51_wdt_rr_register_e rr_register)
{
    NRF_WDT->RREN |= 0x1UL << rr_register;
}

void nrf51_wdt_reload_request_disable(enum nrf51_wdt_rr_register_e rr_register)
{
    NRF_WDT->RREN &= ~(0x1UL << rr_register);
}


void nrf51_wdt_reload_request_set(enum nrf51_wdt_rr_register_e rr_register)
{
    NRF_WDT->RR[rr_register] = NRF51_WDT_RR_VALUE;
}
