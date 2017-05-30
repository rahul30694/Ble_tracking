#ifndef _NRF51_TIMER_H_
#define _NRF51_TIMER_H_

#include "nrf.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NRF51_TIMER_INSTANCE_NUMBER   	3
#define NRF51_TIMER_CHANNEL_NUMBER    	4

enum nrf51_timer_task_e {
	NRF51_TIMER_TASK_START		=	offsetof(NRF_TIMER_Type, TASKS_START),
	NRF51_TIMER_TASK_STOP		=	offsetof(NRF_TIMER_Type, TASKS_STOP),
	NRF51_TIMER_TASK_COUNT		=	offsetof(NRF_TIMER_Type, TASKS_COUNT),
	NRF51_TIMER_TASK_CLEAR		=	offsetof(NRF_TIMER_Type, TASKS_CLEAR),
	NRF51_TIMER_TASK_SHUTDOWN	=	offsetof(NRF_TIMER_Type, TASKS_SHUTDOWN),
	NRF51_TIMER_TASK_CAPTURE_0	=	offsetof(NRF_TIMER_Type, TASKS_CAPTURE[0]),
	NRF51_TIMER_TASK_CAPTURE_1	=	offsetof(NRF_TIMER_Type, TASKS_CAPTURE[1]),
	NRF51_TIMER_TASK_CAPTURE_2	=	offsetof(NRF_TIMER_Type, TASKS_CAPTURE[2]),
	NRF51_TIMER_TASK_CAPTURE_3	=	offsetof(NRF_TIMER_Type, TASKS_CAPTURE[3]),
};

enum nrf51_timer_event_e {
	NRF51_TIMER_EVENT_CMP_0		=	offsetof(NRF_TIMER_Type, EVENTS_COMPARE[0]),
	NRF51_TIMER_EVENT_CMP_1		=	offsetof(NRF_TIMER_Type, EVENTS_COMPARE[1]),
	NRF51_TIMER_EVENT_CMP_2		=	offsetof(NRF_TIMER_Type, EVENTS_COMPARE[2]),
	NRF51_TIMER_EVENT_CMP_3		=	offsetof(NRF_TIMER_Type, EVENTS_COMPARE[3]),
};

enum nrf51_timer_short_mask_e {
    NRF51_TIMER_SHORT_COMPARE0_STOP_MASK = TIMER_SHORTS_COMPARE0_STOP_Msk,
    NRF51_TIMER_SHORT_COMPARE1_STOP_MASK = TIMER_SHORTS_COMPARE1_STOP_Msk,
    NRF51_TIMER_SHORT_COMPARE2_STOP_MASK = TIMER_SHORTS_COMPARE2_STOP_Msk,
    NRF51_TIMER_SHORT_COMPARE3_STOP_MASK = TIMER_SHORTS_COMPARE3_STOP_Msk,
    NRF51_TIMER_SHORT_COMPARE0_CLEAR_MASK = TIMER_SHORTS_COMPARE0_CLEAR_Msk,
    NRF51_TIMER_SHORT_COMPARE1_CLEAR_MASK = TIMER_SHORTS_COMPARE1_CLEAR_Msk,
    NRF51_TIMER_SHORT_COMPARE2_CLEAR_MASK = TIMER_SHORTS_COMPARE2_CLEAR_Msk,
    NRF51_TIMER_SHORT_COMPARE3_CLEAR_MASK = TIMER_SHORTS_COMPARE3_CLEAR_Msk,
};

enum nrf51_timer_bitmode_e {
	NRF51_TIMER_BITMODE_08	=	TIMER_BITMODE_BITMODE_08Bit,
	NRF51_TIMER_BITMODE_16	=	TIMER_BITMODE_BITMODE_16Bit,
	NRF51_TIMER_BITMODE_24	=	TIMER_BITMODE_BITMODE_24Bit,
	NRF51_TIMER_BITMODE_32	=	TIMER_BITMODE_BITMODE_32Bit,
};

enum nrf51_timer_freq_e {
    NRF51_TIMER_FREQ_16MHz=0,
    NRF51_TIMER_FREQ_8MHz, 
    NRF51_TIMER_FREQ_4MHz, 
    NRF51_TIMER_FREQ_2MHz,
    NRF51_TIMER_FREQ_1MHz,
    NRF51_TIMER_FREQ_500kHz,
    NRF51_TIMER_FREQ_250kHz,
    NRF51_TIMER_FREQ_125kHz,
    NRF51_TIMER_FREQ_62500Hz,
    NRF51_TIMER_FREQ_31250Hz
};

enum nrf51_timer_int_mask_e {
    NRF51_TIMER_INT_COMP_0_MASK = TIMER_INTENSET_COMPARE0_Msk,
    NRF51_TIMER_INT_COMP_1_MASK = TIMER_INTENSET_COMPARE1_Msk,
    NRF51_TIMER_INT_COMP_2_MASK = TIMER_INTENSET_COMPARE2_Msk,
    NRF51_TIMER_INT_COMP_3_MASK = TIMER_INTENSET_COMPARE3_Msk,
};

enum nrf51_timer_cc_channel_e {
    NRF51_TIMER_CC_CHANNEL0=0,
    NRF51_TIMER_CC_CHANNEL1,
    NRF51_TIMER_CC_CHANNEL2,
    NRF51_TIMER_CC_CHANNEL3,
};

enum nrf51_timer_mode_e {
	NRF51_TIMER_MODE_TIMER	=	TIMER_MODE_MODE_Timer,
	NRF51_TIMER_MODE_COUNT	=	TIMER_MODE_MODE_Counter,
};


void nrf51_timer_task_trigger(NRF_TIMER_Type *p_timer, enum nrf51_timer_task_e task);

uint32_t *nrf51_timer_task_address_get(NRF_TIMER_Type *p_timer, enum nrf51_timer_task_e task);

void nrf51_timer_event_clear(NRF_TIMER_Type *p_timer, enum nrf51_timer_event_e event);

bool nrf51_timer_event_check(NRF_TIMER_Type *p_timer, enum nrf51_timer_event_e event);

uint32_t *nrf51_timer_event_address_get(NRF_TIMER_Type *p_timer, enum nrf51_timer_event_e event);


void nrf51_timer_shorts_enable(NRF_TIMER_Type *p_timer, uint32_t mask);

void nrf51_timer_shorts_disable(NRF_TIMER_Type *p_timer, uint32_t mask);

void nrf51_timer_mode_set(NRF_TIMER_Type *p_timer, enum nrf51_timer_mode_e mode);


void nrf51_timer_bit_width_set(NRF_TIMER_Type *p_timer, enum nrf51_timer_bitmode_e bitmode);

void nrf51_timer_frequency_set(NRF_TIMER_Type *p_timer, enum nrf51_timer_freq_e freq);

uint32_t nrf51_timer_cc_read(NRF_TIMER_Type *p_timer, enum nrf51_timer_cc_channel_e chan);

void nrf51_timer_cc_write(NRF_TIMER_Type *p_timer, enum nrf51_timer_cc_channel_e chan, uint32_t cc_value);


void nrf51_timer_int_enable(NRF_TIMER_Type *p_timer, uint32_t mask);

void nrf51_timer_int_disable(NRF_TIMER_Type *p_timer, uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif
