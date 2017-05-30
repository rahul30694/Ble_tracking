#ifndef _NRF51_ADC_H_
#define _NRF51_ADC_H_

#include "nrf.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum nrf51_adc_cfg_resolution_e {
    NRF51_ADC_CONFIG_RES_8BIT  = ADC_CONFIG_RES_8bit,
    NRF51_ADC_CONFIG_RES_9BIT  = ADC_CONFIG_RES_9bit,
    NRF51_ADC_CONFIG_RES_10BIT = ADC_CONFIG_RES_10bit,
};

enum nrf51_adc_cfg_scaling_e {
    NRF51_ADC_CONFIG_SCALING_INPUT_FULL_SCALE  = ADC_CONFIG_INPSEL_AnalogInputNoPrescaling,        
    NRF51_ADC_CONFIG_SCALING_INPUT_TWO_THIRDS  = ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling, 
    NRF51_ADC_CONFIG_SCALING_INPUT_ONE_THIRD   = ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling,  
    NRF51_ADC_CONFIG_SCALING_SUPPLY_TWO_THIRDS = ADC_CONFIG_INPSEL_SupplyTwoThirdsPrescaling,  
    NRF51_ADC_CONFIG_SCALING_SUPPLY_ONE_THIRD  = ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling        
};

enum nrf51_adc_cfg_ref_e {
    NRF51_ADC_CONFIG_REF_VBG              = ADC_CONFIG_REFSEL_VBG,                      
    NRF51_ADC_CONFIG_REF_SUPPLY_ONE_HALF  = ADC_CONFIG_REFSEL_SupplyOneHalfPrescaling,  
    NRF51_ADC_CONFIG_REF_SUPPLY_ONE_THIRD = ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling, 
    NRF51_ADC_CONFIG_REF_EXT_REF0         = ADC_CONFIG_REFSEL_External |
                                          ADC_CONFIG_EXTREFSEL_AnalogReference0 << ADC_CONFIG_EXTREFSEL_Pos, 
    NRF51_ADC_CONFIG_REF_EXT_REF1 = ADC_CONFIG_REFSEL_External |
                                  ADC_CONFIG_EXTREFSEL_AnalogReference1 << ADC_CONFIG_EXTREFSEL_Pos, 
};

enum nrf51_adc_cfg_input_e {
    NRF51_ADC_CONFIG_INPUT_DISABLED = ADC_CONFIG_PSEL_Disabled,     
    NRF51_ADC_CONFIG_INPUT_0        = ADC_CONFIG_PSEL_AnalogInput0, 
    NRF51_ADC_CONFIG_INPUT_1        = ADC_CONFIG_PSEL_AnalogInput1, 
    NRF51_ADC_CONFIG_INPUT_2        = ADC_CONFIG_PSEL_AnalogInput2,
    NRF51_ADC_CONFIG_INPUT_3        = ADC_CONFIG_PSEL_AnalogInput3,
    NRF51_ADC_CONFIG_INPUT_4        = ADC_CONFIG_PSEL_AnalogInput4,
    NRF51_ADC_CONFIG_INPUT_5        = ADC_CONFIG_PSEL_AnalogInput5,
    NRF51_ADC_CONFIG_INPUT_6        = ADC_CONFIG_PSEL_AnalogInput6,
    NRF51_ADC_CONFIG_INPUT_7        = ADC_CONFIG_PSEL_AnalogInput7,
};

enum nrf51_adc_task_e {
    NRF51_ADC_TASK_START = offsetof(NRF_ADC_Type, TASKS_START),
    NRF51_ADC_TASK_STOP  = offsetof(NRF_ADC_Type, TASKS_STOP),
};

enum nrf51_adc_event_e {
    NRF51_ADC_EVENT_END = offsetof(NRF_ADC_Type, EVENTS_END),
};

void nrf51_adc_configure(enum nrf51_adc_cfg_resolution_e r, enum nrf51_adc_cfg_scaling_e s, enum nrf51_adc_cfg_ref_e ref);
int32_t nrf51_adc_convert_single(enum nrf51_adc_cfg_input_e input);
void nrf51_adc_input_select(enum nrf51_adc_cfg_input_e input);
int32_t nrf51_adc_result_get(void);
bool nrf51_adc_is_busy(void);
void nrf51_adc_int_enable(uint32_t mask);
void nrf51_adc_int_disable(uint32_t mask);
void nrf51_adc_start(void);
void nrf51_adc_stop(void);
bool nrf51_adc_conversion_finished(void);
void nrf51_adc_conversion_event_clean(void);
uint32_t *nrf51_adc_task_address_get(enum nrf51_adc_task_e task);
uint32_t *nrf51_adc_event_address_get(enum nrf51_adc_event_e event);
void nrf51_adc_task_trigger(enum nrf51_adc_task_e task);
void nrf51_adc_event_clean(enum nrf51_adc_event_e event);

#ifdef __cplusplus
}
#endif

#endif
