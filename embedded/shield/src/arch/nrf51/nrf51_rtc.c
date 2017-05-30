#include "nrf51_rtc.h"

#define NRF51_RTC_TASK_TRIGGER 	(1UL)
#define NRF51_RTC_EVENT_CLEAR  	(0UL)


void nrf51_rtc_cc_set(NRF_RTC_Type *rtc, uint32_t ch, uint32_t cc_val)
{
    rtc->CC[ch] = cc_val;
}

void nrf51_rtc_int_enable(NRF_RTC_Type *rtc, uint32_t mask)
{
    rtc->INTENSET = mask;
}

void nrf51_rtc_int_disable(NRF_RTC_Type *rtc, uint32_t mask)
{
    rtc->INTENCLR = mask;
}

uint32_t nrf51_rtc_event_check(NRF_RTC_Type *rtc, enum nrf51_rtc_event_e event)
{
    return !!(*(volatile uint32_t *)((uint8_t *)rtc + (uint32_t)event));
}

void nrf51_rtc_event_clear(NRF_RTC_Type *p_rtc, enum nrf51_rtc_event_e event)
{
    *((volatile uint32_t *)((uint8_t *)p_rtc + (uint32_t)event)) = NRF51_RTC_EVENT_CLEAR;
}

uint32_t nrf51_rtc_counter_get(NRF_RTC_Type *p_rtc)
{
     return p_rtc->COUNTER;
}

void nrf51_rtc_prescaler_set(NRF_RTC_Type *p_rtc, uint32_t val)
{
    //ASSERT(val <= (RTC_PRESCALER_PRESCALER_Msk >> RTC_PRESCALER_PRESCALER_Pos));
    p_rtc->PRESCALER = val;
}


uint32_t nrf51_rtc_event_address_get(NRF_RTC_Type *p_rtc, enum nrf51_rtc_event_e event)
{
    return (uint32_t)p_rtc + event;
}

uint32_t nrf51_rtc_task_address_get(NRF_RTC_Type *p_rtc, enum nrf51_rtc_task_e task)
{
    return (uint32_t)p_rtc + task;
}

void nrf51_rtc_task_trigger(NRF_RTC_Type *p_rtc, enum nrf51_rtc_task_e task)
{
    *(volatile uint32_t *)((uint8_t *)p_rtc + task) = NRF51_RTC_TASK_TRIGGER;
}

void nrf51_rtc_event_enable(NRF_RTC_Type *p_rtc, uint32_t mask)
{
    p_rtc->EVTENSET = mask;
}

void nrf51_rtc_event_disable(NRF_RTC_Type *p_rtc, uint32_t mask)
{
    p_rtc->EVTENCLR = mask;
}
