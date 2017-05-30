#ifndef _NRF51_WDT_H_
#define _NRF51_WDT_H_

#include "nrf.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


enum nrf51_wdt_task_e {
    NRF_WDT_TASK_START = offsetof(NRF_WDT_Type, TASKS_START),
};

enum nrf51_wdt_event_e {
    NRF51_WDT_EVENT_TIMEOUT = offsetof(NRF_WDT_Type, EVENTS_TIMEOUT),
};

enum nrf51_wdt_behaviour_e {
    NRF51_WDT_BEHAVIOUR_RUN_SLEEP        = WDT_CONFIG_SLEEP_Msk,                      
    NRF51_WDT_BEHAVIOUR_RUN_HALT         = WDT_CONFIG_HALT_Msk,                        
    NRF51_WDT_BEHAVIOUR_RUN_SLEEP_HALT   = WDT_CONFIG_SLEEP_Msk | WDT_CONFIG_HALT_Msk, 
    NRF51_WDT_BEHAVIOUR_PAUSE_SLEEP_HALT = 0, 
};

enum nrf51_wdt_rr_register_e {
    NRF51_WDT_RR0 = 0, 
    NRF51_WDT_RR1,     
    NRF51_WDT_RR2,     
    NRF51_WDT_RR3,     
    NRF51_WDT_RR4,     
    NRF51_WDT_RR5,     
    NRF51_WDT_RR6,     
    NRF51_WDT_RR7,
};


enum nrf51_wdt_int_mask_e {
    NRF51_WDT_INT_TIMEOUT_MASK = WDT_INTENSET_TIMEOUT_Msk,
};

void nrf51_wdt_behaviour_set(enum nrf51_wdt_behaviour_e behaviour);

void nrf51_wdt_task_trigger(enum nrf51_wdt_task_e task);

void nrf51_wdt_event_clear(enum nrf51_wdt_event_e event);

bool nrf51_wdt_event_check(enum nrf51_wdt_event_e event);


void nrf51_wdt_int_enable(uint32_t mask);


void nrf51_wdt_int_disable(uint32_t mask);
uint32_t nrf51_wdt_task_address_get(enum nrf51_wdt_task_e task);

uint32_t nrf51_wdt_event_address_get(enum nrf51_wdt_event_e event);

bool nrf51_wdt_started(void);

bool nrf51_wdt_request_status(enum nrf51_wdt_rr_register_e rr_register);

void nrf51_wdt_reload_value_set(uint32_t reload_value);


void nrf51_wdt_reload_request_enable(enum nrf51_wdt_rr_register_e rr_register);

void nrf51_wdt_reload_request_disable(enum nrf51_wdt_rr_register_e rr_register);


void nrf51_wdt_reload_request_set(enum nrf51_wdt_rr_register_e rr_register);


#ifdef __cplusplus
}
#endif

#endif