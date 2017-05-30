#ifndef _NRF51_CLOCK_H_
#define _NRF51_CLOCK_H_

#include "nrf.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum nrf51_clock_lf_src_e {
    NRF51_CLOCK_LF_SRC_RC    = CLOCK_LFCLKSRC_SRC_RC,
    NRF51_CLOCK_LF_SRC_XTAL  = CLOCK_LFCLKSRC_SRC_Xtal,
    NRF51_CLOCK_LF_SRC_SYNTH = CLOCK_LFCLKSRC_SRC_Synth,
};

enum nrf51_clock_hf_src_e {
    NRF51_CLOCK_HF_SRC_LOW_ACCURACY   = CLOCK_HFCLKSTAT_SRC_RC,
    NRF51_CLOCK_HF_SRC_HIGH_ACCURACY  = CLOCK_HFCLKSTAT_SRC_Xtal,
};

enum nrf51_clock_start_task_status_e {
    NRF51_CLOCK_START_TASK_NOT_TRIGGERED = CLOCK_LFCLKRUN_STATUS_NotTriggered, 
    NRF51_CLOCK_START_TASK_TRIGGERED     = CLOCK_LFCLKRUN_STATUS_Triggered,
};

enum nrf51_clock_xtalfreq_e {
    NRF51_CLOCK_XTALFREQ_Default = CLOCK_XTALFREQ_XTALFREQ_16MHz,
    NRF51_CLOCK_XTALFREQ_16MHz   = CLOCK_XTALFREQ_XTALFREQ_16MHz,
    NRF51_CLOCK_XTALFREQ_32MHz   = CLOCK_XTALFREQ_XTALFREQ_32MHz,
};


enum nrf51_clock_int_mask_e {
    NRF51_CLOCK_INT_HF_STARTED_MASK = CLOCK_INTENSET_HFCLKSTARTED_Msk,
    NRF51_CLOCK_INT_LF_STARTED_MASK = CLOCK_INTENSET_LFCLKSTARTED_Msk,
    NRF51_CLOCK_INT_DONE_MASK       = CLOCK_INTENSET_DONE_Msk, 
    NRF51_CLOCK_INT_CTTO_MASK       = CLOCK_INTENSET_CTTO_Msk,
};


enum nrf51_clock_task_e {
    NRF51_CLOCK_TASK_HFCLKSTART = offsetof(NRF_CLOCK_Type, TASKS_HFCLKSTART),
    NRF51_CLOCK_TASK_HFCLKSTOP  = offsetof(NRF_CLOCK_Type, TASKS_HFCLKSTOP),
    NRF51_CLOCK_TASK_LFCLKSTART = offsetof(NRF_CLOCK_Type, TASKS_LFCLKSTART),
    NRF51_CLOCK_TASK_LFCLKSTOP  = offsetof(NRF_CLOCK_Type, TASKS_LFCLKSTOP),
    NRF51_CLOCK_TASK_CAL        = offsetof(NRF_CLOCK_Type, TASKS_CAL),
    NRF51_CLOCK_TASK_CTSTART    = offsetof(NRF_CLOCK_Type, TASKS_CTSTART),
    NRF51_CLOCK_TASK_CTSTOP     = offsetof(NRF_CLOCK_Type, TASKS_CTSTOP),
};                                                        

enum nrf51_clock_event_e {
    NRF51_CLOCK_EVENT_HFCLKSTARTED = offsetof(NRF_CLOCK_Type, EVENTS_HFCLKSTARTED),
    NRF51_CLOCK_EVENT_LFCLKSTARTED = offsetof(NRF_CLOCK_Type, EVENTS_LFCLKSTARTED),
    NRF51_CLOCK_EVENT_DONE         = offsetof(NRF_CLOCK_Type, EVENTS_DONE),
    NRF51_CLOCK_EVENT_CTTO         = offsetof(NRF_CLOCK_Type, EVENTS_CTTO),
};  


void nrf51_hfclk_select(enum nrf51_clock_hf_src_e src);
void nrf51_hfclk_start();

void nrf51_hfclk_stop();
bool nrf51_hfclk_is_running();
void nrf51_lfclk_select(enum nrf51_clock_lf_src_e src);

void nrf51_lfclk_start();

void nrf51_lfclk_stop();  

bool nrf51_lfclk_is_running();
void nrf51_clk_int_enable(enum nrf51_clock_int_mask_e mask);

void nrf51_clk_int_disable(enum nrf51_clock_int_mask_e mask);

void nrf51_lfclk_cal();

void nrf51_lfclk_start_cal_timer(uint8_t timer);
void nrf51_clk_task_trigger(enum nrf51_clock_task_e task);
bool nrf51_clk_event_read(enum nrf51_clock_event_e event);
void nrf51_clk_event_clear(enum nrf51_clock_event_e event);
uint32_t nrf51_clk_task_address_get(enum nrf51_clock_task_e task);
uint32_t nrf51_clk_event_address_get(enum nrf51_clock_event_e event);
enum nrf51_clock_lf_src_e nrf51_clock_lf_src_get(void);
enum nrf51_clock_hf_src_e nrf51_clock_hf_src_get(void);
void nrf51_clock_xtalfreq_set(enum nrf51_clock_xtalfreq_e xtalfreq);
enum nrf51_clock_xtalfreq_e nrf51_clock_xtalfreq_get(void);

#ifdef __cplusplus
}
#endif

#endif
