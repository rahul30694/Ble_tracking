#ifndef _NRF51_TEMP_H_
#define _NRF51_TEMP_H_

#include "nrf.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum nrf51_temp_task_e {
	NRF51_TEMP_TASK_START,
	NRF51_TEMP_TASK_STOP,
};

enum nrf51_temp_event_e {
	NRF51_TEMP_EVENT_DATARDY,
};

enum nrf51_temp_int_mask_e {
	NRF51_TEMP_INT_DATARDY_MASK,
};

void nrf51_temp_task_trigger(enum nrf51_temp_task_e task);
bool nrf51_temp_event_read(enum nrf51_temp_event_e event);

void nrf51_temp_event_clear(enum nrf51_temp_event_e event);
void nrf51_temp_int_enable(uint32_t mask);

void nrf51_temp_int_disable(uint32_t mask);

void nrf51_temp_init(void);
int32_t nrf51_temp_read(void);

#ifdef __cplusplus
}
#endif

#endif