#include "time.h"

#include "nrf51/nrf51_timer.h"
#include "nrf51/nrf51_rtc.h"
#include "common/nrf_drv_common.h"
#include "nrf51/nrf51_clock.h"

#include <stdint.h>
#include <stdio.h>

#define TIMER_OVERFLOW_MAX	999UL
volatile uint32_t _ms_escaped=0x00;

void timer_init()
{
	_ms_escaped = 0x0UL;
	nrf51_timer_task_trigger(NRF_TIMER0, NRF51_TIMER_TASK_STOP);
	nrf51_timer_mode_set(NRF_TIMER0, NRF51_TIMER_MODE_TIMER);
	nrf51_timer_frequency_set(NRF_TIMER0, NRF51_TIMER_FREQ_1MHz);

	nrf51_timer_bit_width_set(NRF_TIMER0, NRF51_TIMER_BITMODE_16);
	nrf51_timer_int_enable(NRF_TIMER0, NRF51_TIMER_INT_COMP_0_MASK);
	nrf_drv_common_irq_enable(TIMER0_IRQn, 3);
	nrf51_timer_task_trigger(NRF_TIMER0, NRF51_TIMER_TASK_START);
	nrf51_timer_cc_write(NRF_TIMER0, NRF51_TIMER_CC_CHANNEL0, TIMER_OVERFLOW_MAX);
	nrf51_timer_shorts_enable(NRF_TIMER0, NRF51_TIMER_SHORT_COMPARE0_CLEAR_MASK);
}

#ifdef __cplusplus
extern "C" {
#endif

void TIMER0_IRQHandler(void)
{
	if (nrf51_timer_event_check(NRF_TIMER0, NRF51_TIMER_EVENT_CMP_0)) {
		_ms_escaped++;
		nrf51_timer_event_clear(NRF_TIMER0, NRF51_TIMER_EVENT_CMP_0);
	}
}

#ifdef __cplusplus
}
#endif

unsigned long micros(void)
{
	nrf51_timer_task_trigger(NRF_TIMER0, NRF51_TIMER_TASK_CAPTURE_1);
	uint16_t _us_escaped = nrf51_timer_cc_read(NRF_TIMER0, NRF51_TIMER_CC_CHANNEL1);

	return ((_ms_escaped << 10) + _us_escaped);
}

unsigned long millis(void)
{
	return (_ms_escaped);
}

void (*systick_event_handler)(void);

#define CALTIME_IN_MS(x)	(unsigned char)((x)/250)

void systick_clock_cfg()
{
	nrf51_clock_xtalfreq_set(NRF51_CLOCK_XTALFREQ_Default);
	nrf51_lfclk_select(NRF51_CLOCK_LF_SRC_SYNTH);
	nrf51_lfclk_start();
	nrf51_clk_event_clear(NRF51_CLOCK_EVENT_HFCLKSTARTED);
	nrf51_clk_event_clear(NRF51_CLOCK_EVENT_LFCLKSTARTED);
	nrf51_clk_event_clear(NRF51_CLOCK_EVENT_DONE);
	nrf51_clk_event_clear(NRF51_CLOCK_EVENT_CTTO);
	return;
	nrf51_lfclk_start_cal_timer(CALTIME_IN_MS(2000));
	//while (!nrf51_clk_event_read(NRF51_CLOCK_EVENT_DONE));
}

void systick_init_prescaler(uint32_t val, void (*event_handler)(void))
{
	systick_clock_cfg();

	nrf_drv_common_irq_enable(RTC0_IRQn, 3);
	nrf51_rtc_prescaler_set(NRF_RTC0, val);
	systick_event_handler = event_handler;


	nrf51_rtc_event_clear(NRF_RTC0, NRF51_RTC_EVENT_TICK);
	nrf51_rtc_event_enable(NRF_RTC0, NRF51_RTC_INT_TICK_MASK);
	nrf51_rtc_int_enable(NRF_RTC0, NRF51_RTC_INT_TICK_MASK);
	nrf51_rtc_task_trigger(NRF_RTC0, NRF51_RTC_TASK_START);
}

unsigned long systick(void)
{
	return nrf51_rtc_counter_get(NRF_RTC0);
}

#ifdef __cplusplus
extern "C" {
#endif

void RTC0_IRQHandler(void)
{
	if (nrf51_rtc_event_check(NRF_RTC0, NRF51_RTC_EVENT_TICK)) {
		nrf51_rtc_event_clear(NRF_RTC0, NRF51_RTC_EVENT_TICK);
		if (systick_event_handler)
			systick_event_handler();
	}
}

#ifdef __cplusplus
}
#endif
