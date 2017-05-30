#include "nrf51_timer.h"

#define NRF51_TIMER_TASK_TRIGGER 		(1UL)
#define NRF51_TIMER_EVENT_CLEAR  		(0UL)


void nrf51_timer_task_trigger(NRF_TIMER_Type *p_timer, enum nrf51_timer_task_e task)
{
    *((volatile uint32_t *)((uint8_t *)p_timer + (uint32_t)task)) = NRF51_TIMER_TASK_TRIGGER;
}


uint32_t *nrf51_timer_task_address_get(NRF_TIMER_Type *p_timer, enum nrf51_timer_task_e task)
{
    return (uint32_t *)((uint8_t *)p_timer + (uint32_t)task);
}

void nrf51_timer_event_clear(NRF_TIMER_Type *p_timer, enum nrf51_timer_event_e event)
{
    *((volatile uint32_t *)((uint8_t *)p_timer + (uint32_t)event)) = NRF51_TIMER_EVENT_CLEAR;
}

bool nrf51_timer_event_check(NRF_TIMER_Type *p_timer, enum nrf51_timer_event_e event)
{
    return (bool)*(volatile uint32_t *)((uint8_t *)p_timer + (uint32_t)event);
}

uint32_t *nrf51_timer_event_address_get(NRF_TIMER_Type *p_timer, enum nrf51_timer_event_e event)
{
    return (uint32_t *)((uint8_t *)p_timer + (uint32_t)event);
}


void nrf51_timer_shorts_enable(NRF_TIMER_Type *p_timer, uint32_t mask)
{
    p_timer->SHORTS |= mask;
}

void nrf51_timer_shorts_disable(NRF_TIMER_Type *p_timer, uint32_t mask)
{
	p_timer->SHORTS &= ~(mask);
}

void nrf51_timer_mode_set(NRF_TIMER_Type *p_timer, enum nrf51_timer_mode_e mode)
{
    p_timer->MODE = (p_timer->MODE & ~TIMER_MODE_MODE_Msk) |
                       ((mode << TIMER_MODE_MODE_Pos) & TIMER_MODE_MODE_Msk);
}


void nrf51_timer_bit_width_set(NRF_TIMER_Type *p_timer, enum nrf51_timer_bitmode_e bitmode)
{
    p_timer->BITMODE = (p_timer->BITMODE & ~TIMER_BITMODE_BITMODE_Msk) |
                          ((bitmode << TIMER_BITMODE_BITMODE_Pos) & TIMER_BITMODE_BITMODE_Msk);
}

void nrf51_timer_frequency_set(NRF_TIMER_Type *p_timer, enum nrf51_timer_freq_e freq)
{
    p_timer->PRESCALER = (p_timer->PRESCALER & ~TIMER_PRESCALER_PRESCALER_Msk) |
                            ((freq << TIMER_PRESCALER_PRESCALER_Pos) & TIMER_PRESCALER_PRESCALER_Msk);
}

uint32_t nrf51_timer_cc_read(NRF_TIMER_Type *p_timer, enum nrf51_timer_cc_channel_e chan)
{
    return (uint32_t)p_timer->CC[chan];
}

void nrf51_timer_cc_write(NRF_TIMER_Type *p_timer, enum nrf51_timer_cc_channel_e chan, uint32_t cc_value)
{
    p_timer->CC[chan] = cc_value;
}


void nrf51_timer_int_enable(NRF_TIMER_Type *p_timer, uint32_t mask)
{
    p_timer->INTENSET = mask;
}

void nrf51_timer_int_disable(NRF_TIMER_Type *p_timer, uint32_t mask)
{
    p_timer->INTENCLR = mask;
}
