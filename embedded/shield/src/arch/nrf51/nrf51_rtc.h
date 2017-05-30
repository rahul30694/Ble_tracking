#ifndef _NRF51_RTC_H_
#define _NRF51_RTC_H_

#include "nrf.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NRF51_RTC_CHANNEL_NUM 	4

#define NRF51_RTC_INPUT_FREQ 	32768 


#define NRF51_RTC_CHANNEL_INT_MASK(ch)    ((uint32_t)NRF_RTC_INT_COMPARE0_MASK << ch)
#define NRF51_RTC_CHANNEL_EVENT_ADDR(ch)  (enum nrf51_rtc_event_e)(NRF51_RTC_EVENT_CMP_0 + ch * sizeof(uint32_t))


enum nrf51_rtc_task_e {
	NRF51_RTC_TASK_START		= offsetof(NRF_RTC_Type, TASKS_START),
	NRF51_RTC_TASK_STOP 		= offsetof(NRF_RTC_Type, TASKS_STOP),
	NRF51_RTC_TASK_CLEAR 		= offsetof(NRF_RTC_Type, TASKS_CLEAR),
	NRF51_RTC_TASK_TRIGOVRFLW 	= offsetof(NRF_RTC_Type, TASKS_TRIGOVRFLW),
};

enum nrf51_rtc_event_e {
	NRF51_RTC_EVENT_TICK 	= offsetof(NRF_RTC_Type, EVENTS_TICK),
	NRF51_RTC_EVENT_OVRFLW 	= offsetof(NRF_RTC_Type, EVENTS_OVRFLW),
	NRF51_RTC_EVENT_CMP_0 	= offsetof(NRF_RTC_Type, EVENTS_COMPARE[0]),
	NRF51_RTC_EVENT_CMP_1 	= offsetof(NRF_RTC_Type, EVENTS_COMPARE[1]),
	NRF51_RTC_EVENT_CMP_2 	= offsetof(NRF_RTC_Type, EVENTS_COMPARE[2]),
	NRF51_RTC_EVENT_CMP_3 	= offsetof(NRF_RTC_Type, EVENTS_COMPARE[3]),
};

enum nrf51_rtc_int_e {
    NRF51_RTC_INT_TICK_MASK     = RTC_INTENSET_TICK_Msk,     
    NRF51_RTC_INT_OVERFLOW_MASK = RTC_INTENSET_OVRFLW_Msk,   
    NRF51_RTC_INT_CMP_0_MASK 	= RTC_INTENSET_COMPARE0_Msk,
    NRF51_RTC_INT_CMP_1_MASK 	= RTC_INTENSET_COMPARE1_Msk,
    NRF51_RTC_INT_CMP_2_MASK	= RTC_INTENSET_COMPARE2_Msk, 
    NRF51_RTC_INT_CMP_3_MASK 	= RTC_INTENSET_COMPARE3_Msk,
};


void nrf51_rtc_cc_set(NRF_RTC_Type *rtc, uint32_t ch, uint32_t cc_val);
void nrf51_rtc_int_enable(NRF_RTC_Type *rtc, uint32_t mask);
void nrf51_rtc_int_disable(NRF_RTC_Type *rtc, uint32_t mask);
uint32_t nrf51_rtc_event_check(NRF_RTC_Type *rtc, enum nrf51_rtc_event_e event);
void nrf51_rtc_event_clear(NRF_RTC_Type *p_rtc, enum nrf51_rtc_event_e event);
uint32_t nrf51_rtc_counter_get(NRF_RTC_Type *p_rtc);
void nrf51_rtc_prescaler_set(NRF_RTC_Type *p_rtc, uint32_t val);
uint32_t nrf51_rtc_event_address_get(NRF_RTC_Type *p_rtc, enum nrf51_rtc_event_e event);
uint32_t nrf51_rtc_task_address_get(NRF_RTC_Type *p_rtc, enum nrf51_rtc_task_e task);
void nrf51_rtc_task_trigger(NRF_RTC_Type *p_rtc, enum nrf51_rtc_task_e task);
void nrf51_rtc_event_enable(NRF_RTC_Type *p_rtc, uint32_t mask);
void nrf51_rtc_event_disable(NRF_RTC_Type *p_rtc, uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif
