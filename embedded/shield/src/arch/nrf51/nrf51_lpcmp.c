#include "nrf51_lpcmp.h"

#define NRF51_LPCMP_TASK_TRIGGER       	(1UL)
#define NRF51_LPCMP_EVENT_CLEAR    		(0UL)



void nrf51_lpcmp_cfg(enum nrf51_lpcmp_ref_e ref, enum nrf51_lpcmp_detect_e detect)
{
    NRF_LPCOMP->TASKS_STOP = NRF51_LPCMP_TASK_TRIGGER;
    NRF_LPCOMP->ENABLE = LPCOMP_ENABLE_ENABLE_Disabled << LPCOMP_ENABLE_ENABLE_Pos;
    NRF_LPCOMP->REFSEL = (ref << LPCOMP_REFSEL_REFSEL_Pos) & LPCOMP_REFSEL_REFSEL_Msk;

    if ((ref & LPCOMP_REFSEL_REFSEL_ARef) == LPCOMP_REFSEL_REFSEL_ARef) {
        uint32_t extref = ref >> 16;
        NRF_LPCOMP->EXTREFSEL = (extref << LPCOMP_EXTREFSEL_EXTREFSEL_Pos) & LPCOMP_EXTREFSEL_EXTREFSEL_Msk;
    }

    NRF_LPCOMP->ANADETECT =
        (detect << LPCOMP_ANADETECT_ANADETECT_Pos) & LPCOMP_ANADETECT_ANADETECT_Msk;
    NRF_LPCOMP->SHORTS   = 0;
    NRF_LPCOMP->INTENCLR = LPCOMP_INTENCLR_CROSS_Msk | LPCOMP_INTENCLR_UP_Msk |
                           LPCOMP_INTENCLR_DOWN_Msk | LPCOMP_INTENCLR_READY_Msk;
}


void nrf51_lpcmp_input_select(enum nrf51_lpcmp_input_e input)
{
    uint32_t saved_state = NRF_LPCOMP->ENABLE;

    NRF_LPCOMP->ENABLE = LPCOMP_ENABLE_ENABLE_Disabled << LPCOMP_ENABLE_ENABLE_Pos;
    NRF_LPCOMP->PSEL   = ((uint32_t)input << LPCOMP_PSEL_PSEL_Pos) | (NRF_LPCOMP->PSEL & ~LPCOMP_PSEL_PSEL_Msk);
    NRF_LPCOMP->ENABLE = saved_state;
}


void nrf51_lpcmp_enable(void)
{
    NRF_LPCOMP->ENABLE = LPCOMP_ENABLE_ENABLE_Enabled << LPCOMP_ENABLE_ENABLE_Pos;
    NRF_LPCOMP->EVENTS_READY = NRF51_LPCMP_EVENT_CLEAR;
    NRF_LPCOMP->EVENTS_DOWN  = NRF51_LPCMP_EVENT_CLEAR;
    NRF_LPCOMP->EVENTS_UP    = NRF51_LPCMP_EVENT_CLEAR;
    NRF_LPCOMP->EVENTS_CROSS = NRF51_LPCMP_EVENT_CLEAR;
}


void nrf51_lpcmp_disable(void)
{
    NRF_LPCOMP->ENABLE = LPCOMP_ENABLE_ENABLE_Disabled << LPCOMP_ENABLE_ENABLE_Pos;
}


uint32_t nrf51_lpcmp_result_get(void)
{
    return (uint32_t)NRF_LPCOMP->RESULT;
}


void nrf51_lpcmp_int_enable(uint32_t mask)
{
    NRF_LPCOMP->INTENSET = mask;
}

void nrf51_lpcmp_int_disable(uint32_t mask)
{
    NRF_LPCOMP->INTENCLR = mask;
}


uint32_t *nrf51_lpcmp_task_address_get(enum nrf51_lpcmp_task_e task)
{
    return (uint32_t *)((uint8_t *)NRF_LPCOMP + task);
}

uint32_t *nrf51_lpcmp_event_address_get(enum nrf51_lpcmp_event_e event)
{
    return (uint32_t *)((uint8_t *)NRF_LPCOMP + event);
}


void nrf51_lpcmp_shorts_enable(uint32_t mask)
{
    NRF_LPCOMP->SHORTS |= mask;
}


void nrf51_lpcmp_shorts_disable(uint32_t mask)
{
    NRF_LPCOMP->SHORTS &= ~mask;
}

void nrf51_lpcmp_task_trigger(enum nrf51_lpcmp_task_e task)
{
    *( (volatile uint32_t *)( (uint8_t *)NRF_LPCOMP + task) ) = NRF51_LPCMP_TASK_TRIGGER;
}

void nrf51_lpcmp_event_clear(enum nrf51_lpcmp_event_e event)
{
    *( (volatile uint32_t *)( (uint8_t *)NRF_LPCOMP + event) ) = NRF51_LPCMP_EVENT_CLEAR;
}

bool nrf51_lpcmp_event_check(enum nrf51_lpcmp_event_e event)
{
    return (bool) (*(volatile uint32_t *)( (uint8_t *)NRF_LPCOMP + event));
}

