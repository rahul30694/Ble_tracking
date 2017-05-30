#include "nrf51_clock.h"

#define NRF51_CLOCK_TASK_TRIGGER (1UL)
#define NRF51_CLOCK_EVENT_CLEAR  (0UL)


void nrf51_hfclk_select(enum nrf51_clock_hf_src_e src)
{
	NRF_CLOCK->XTALFREQ = src;
}

void nrf51_clock_xtalfreq_set(enum nrf51_clock_xtalfreq_e xtalfreq)
{
	NRF_CLOCK->XTALFREQ =
        (uint32_t)((xtalfreq << CLOCK_XTALFREQ_XTALFREQ_Pos) & CLOCK_XTALFREQ_XTALFREQ_Msk);
}

void nrf51_hfclk_start()
{
	nrf51_clk_task_trigger(NRF51_CLOCK_TASK_HFCLKSTART);
}

void nrf51_hfclk_stop()
{
	nrf51_clk_task_trigger(NRF51_CLOCK_TASK_HFCLKSTOP);
}

bool nrf51_hfclk_is_running()
{
	return !!(NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk);
}

void nrf51_lfclk_select(enum nrf51_clock_lf_src_e src)
{
	NRF_CLOCK->LFCLKSRC = (uint32_t)((src << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
}

void nrf51_lfclk_start()
{
	nrf51_clk_task_trigger(NRF51_CLOCK_TASK_LFCLKSTART);
}

void nrf51_lfclk_stop()
{
	nrf51_clk_task_trigger(NRF51_CLOCK_TASK_LFCLKSTOP);
}	

bool nrf51_lfclk_is_running()
{
	return !!(NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk);
}

void nrf51_clk_int_enable(enum nrf51_clock_int_mask_e mask)
{
	NRF_CLOCK->INTENSET = mask;
}

void nrf51_clk_int_disable(enum nrf51_clock_int_mask_e mask)
{
	NRF_CLOCK->INTENCLR = mask;
}

void nrf51_lfclk_cal()
{
	nrf51_clk_task_trigger(NRF51_CLOCK_TASK_CAL);
}

void nrf51_lfclk_start_cal_timer(uint8_t timer)
{
	NRF_CLOCK->CTIV = timer;
	nrf51_clk_task_trigger(NRF51_CLOCK_TASK_CAL);
}

void nrf51_clk_task_trigger(enum nrf51_clock_task_e task)
{
	*((volatile uint32_t *)((uint8_t *)NRF_CLOCK + task)) = NRF51_CLOCK_TASK_TRIGGER;
}


bool nrf51_clk_event_read(enum nrf51_clock_event_e event)
{
	return !!(*((volatile uint32_t *)((uint8_t *)NRF_CLOCK + event)));
}

void nrf51_clk_event_clear(enum nrf51_clock_event_e event)
{
	*((volatile uint32_t *)((uint8_t *)NRF_CLOCK + event)) = NRF51_CLOCK_EVENT_CLEAR;
}

uint32_t nrf51_clk_task_address_get(enum nrf51_clock_task_e task)
{
    return ((uint32_t )NRF_CLOCK + task);
}

uint32_t nrf51_clk_event_address_get(enum nrf51_clock_event_e event)
{
    return ((uint32_t)NRF_CLOCK + event);
}

enum nrf51_clock_lf_src_e nrf51_clock_lf_src_get(void)
{
    return (enum nrf51_clock_lf_src_e)((NRF_CLOCK->LFCLKSTAT & CLOCK_LFCLKSTAT_SRC_Msk) >> CLOCK_LFCLKSTAT_SRC_Pos);
}

enum nrf51_clock_hf_src_e nrf51_clock_hf_src_get(void)
{
    return (enum nrf51_clock_hf_src_e)((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Msk) >> CLOCK_HFCLKSTAT_SRC_Pos);
}

enum nrf51_clock_xtalfreq_e nrf51_clock_xtalfreq_get(void)
{
    return (enum nrf51_clock_xtalfreq_e)((NRF_CLOCK->XTALFREQ &
                                       CLOCK_XTALFREQ_XTALFREQ_Msk) >> CLOCK_XTALFREQ_XTALFREQ_Pos);

}
