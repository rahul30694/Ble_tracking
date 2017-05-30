#ifndef _NRF51_LPCMP_H_
#define _NRF51_LPCMP_H_

#include "nrf.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


enum nrf51_lpcmp_ref_e {
    NRF51_LPCMP_REF_SUPPLY_1_8   = LPCOMP_REFSEL_REFSEL_SupplyOneEighthPrescaling,    
    NRF51_LPCMP_REF_SUPPLY_2_8   = LPCOMP_REFSEL_REFSEL_SupplyTwoEighthsPrescaling,  
    NRF51_LPCMP_REF_SUPPLY_3_8   = LPCOMP_REFSEL_REFSEL_SupplyThreeEighthsPrescaling,
    NRF51_LPCMP_REF_SUPPLY_4_8   = LPCOMP_REFSEL_REFSEL_SupplyFourEighthsPrescaling,
    NRF51_LPCMP_REF_SUPPLY_5_8   = LPCOMP_REFSEL_REFSEL_SupplyFiveEighthsPrescaling,
    NRF51_LPCMP_REF_SUPPLY_6_8   = LPCOMP_REFSEL_REFSEL_SupplySixEighthsPrescaling,
    NRF51_LPCMP_REF_SUPPLY_7_8   = LPCOMP_REFSEL_REFSEL_SupplySevenEighthsPrescaling,
    NRF51_LPCMP_REF_EXT_REF0        = LPCOMP_REFSEL_REFSEL_ARef |
                       (LPCOMP_EXTREFSEL_EXTREFSEL_AnalogReference0 << 16),
    NRF51_LPCMP_CONFIG_REF_EXT_REF1 = LPCOMP_REFSEL_REFSEL_ARef |
                        (LPCOMP_EXTREFSEL_EXTREFSEL_AnalogReference1 << 16),
};

enum nrf51_lpcmp_input_e {
    NRF51_LPCMP_INPUT_0 = LPCOMP_PSEL_PSEL_AnalogInput0,
    NRF51_LPCMP_INPUT_1 = LPCOMP_PSEL_PSEL_AnalogInput1,
    NRF51_LPCMP_INPUT_2 = LPCOMP_PSEL_PSEL_AnalogInput2, 
    NRF51_LPCMP_INPUT_3 = LPCOMP_PSEL_PSEL_AnalogInput3, 
    NRF51_LPCMP_INPUT_4 = LPCOMP_PSEL_PSEL_AnalogInput4,
    NRF51_LPCMP_INPUT_5 = LPCOMP_PSEL_PSEL_AnalogInput5,
    NRF51_LPCMP_INPUT_6 = LPCOMP_PSEL_PSEL_AnalogInput6,
    NRF51_LPCMP_INPUT_7 = LPCOMP_PSEL_PSEL_AnalogInput7,
};

enum nrf51_lpcmp_detect_e {
    NRF51_LPCMP_DETECT_CROSS = LPCOMP_ANADETECT_ANADETECT_Cross, 
    NRF51_LPCMP_DETECT_UP    = LPCOMP_ANADETECT_ANADETECT_Up,  
    NRF51_LPCMP_DETECT_DOWN  = LPCOMP_ANADETECT_ANADETECT_Down,
};

enum nrf51_lpcmp_task_e {
    NRF51_LPCMP_TASK_START  = offsetof(NRF_LPCOMP_Type, TASKS_START),
    NRF51_LPCMP_TASK_STOP   = offsetof(NRF_LPCOMP_Type, TASKS_STOP), 
    NRF51_LPCMP_TASK_SAMPLE = offsetof(NRF_LPCOMP_Type, TASKS_SAMPLE),
};                                            


enum nrf51_lpcmp_event_e {
    NRF51_LPCMP_EVENT_READY = offsetof(NRF_LPCOMP_Type, EVENTS_READY), 
    NRF51_LPCMP_EVENT_DOWN  = offsetof(NRF_LPCOMP_Type, EVENTS_DOWN),
    NRF51_LPCMP_EVENT_UP    = offsetof(NRF_LPCOMP_Type, EVENTS_UP), 
    NRF51_LPCMP_EVENT_CROSS = offsetof(NRF_LPCOMP_Type, EVENTS_CROSS),
};                                                

enum nrf51_lpcmp_short_mask_e {
    NRF51_LPCMP_SHORT_CROSS_STOP_MASK   = LPCOMP_SHORTS_CROSS_STOP_Msk,  
    NRF51_LPCMP_SHORT_UP_STOP_MASK      = LPCOMP_SHORTS_UP_STOP_Msk,     
    NRF51_LPCMP_SHORT_DOWN_STOP_MASK    = LPCOMP_SHORTS_DOWN_STOP_Msk,  
    NRF51_LPCMP_SHORT_READY_STOP_MASK   = LPCOMP_SHORTS_READY_STOP_Msk,  
    NRF51_LPCMP_SHORT_READY_SAMPLE_MASK = LPCOMP_SHORTS_READY_SAMPLE_Msk,
};


void nrf51_lpcmp_cfg(enum nrf51_lpcmp_ref_e ref, enum nrf51_lpcmp_detect_e detect);
void nrf51_lpcmp_input_select(enum nrf51_lpcmp_input_e input);
void nrf51_lpcmp_enable(void);
void nrf51_lpcmp_disable(void);
uint32_t nrf51_lpcmp_result_get(void);
void nrf51_lpcmp_int_enable(uint32_t mask);
void nrf51_lpcmp_int_disable(uint32_t mask);
uint32_t *nrf51_lpcmp_task_address_get(enum nrf51_lpcmp_task_e task);
uint32_t *nrf51_lpcmp_event_address_get(enum nrf51_lpcmp_event_e event);
void nrf51_lpcmp_shorts_enable(uint32_t mask);
void nrf51_lpcmp_shorts_disable(uint32_t mask);
void nrf51_lpcmp_task_trigger(enum nrf51_lpcmp_task_e task);
void nrf51_lpcmp_event_clear(enum nrf51_lpcmp_event_e event);
bool nrf51_lpcmp_event_check(enum nrf51_lpcmp_event_e event);

#ifdef __cplusplus
}
#endif

#endif