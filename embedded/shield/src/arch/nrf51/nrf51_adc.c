#include "nrf51_adc.h"

#define NRF51_ADC_TASK_TRIGGER 		(1UL)
#define NRF51_ADC_EVENT_CLEAR  		(0UL)


void nrf51_adc_configure(enum nrf51_adc_cfg_resolution_e r, enum nrf51_adc_cfg_scaling_e s, enum nrf51_adc_cfg_ref_e ref)
{
    uint32_t config_reg = 0;

    config_reg |= ((uint32_t)r << ADC_CONFIG_RES_Pos) & ADC_CONFIG_RES_Msk;
    config_reg |= ((uint32_t)s << ADC_CONFIG_INPSEL_Pos) & ADC_CONFIG_INPSEL_Msk;
    config_reg |= ((uint32_t)ref << ADC_CONFIG_REFSEL_Pos) & ADC_CONFIG_REFSEL_Msk;

    if (ref & ADC_CONFIG_EXTREFSEL_Msk) {
        config_reg |= ref & ADC_CONFIG_EXTREFSEL_Msk;
    }
    nrf51_adc_input_select(NRF51_ADC_CONFIG_INPUT_DISABLED);

    NRF_ADC->CONFIG = config_reg | (NRF_ADC->CONFIG & ADC_CONFIG_PSEL_Msk);
}

int32_t nrf51_adc_convert_single(enum nrf51_adc_cfg_input_e input)
{
    int32_t val;

    nrf51_adc_input_select(input);
    nrf51_adc_start();

    while (!nrf51_adc_conversion_finished());
    
    nrf51_adc_conversion_event_clean();
    val = nrf51_adc_result_get();
    nrf51_adc_stop();
    return val;
}

void nrf51_adc_input_select(enum nrf51_adc_cfg_input_e input)
{
    NRF_ADC->CONFIG =
        ((uint32_t)input << ADC_CONFIG_PSEL_Pos) | (NRF_ADC->CONFIG & ~ADC_CONFIG_PSEL_Msk);

    if (input != NRF51_ADC_CONFIG_INPUT_DISABLED) {
        NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled << ADC_ENABLE_ENABLE_Pos;
    } else {
        NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Disabled << ADC_ENABLE_ENABLE_Pos;
    }
}

int32_t nrf51_adc_result_get(void)
{
    return (int32_t)NRF_ADC->RESULT;
}


bool nrf51_adc_is_busy(void)
{
    return ( (NRF_ADC->BUSY & ADC_BUSY_BUSY_Msk) == ADC_BUSY_BUSY_Msk);
}

void nrf51_adc_int_enable(uint32_t mask)
{
    NRF_ADC->INTENSET = mask;
}

void nrf51_adc_int_disable(uint32_t mask)
{
    NRF_ADC->INTENCLR = mask;
}

void nrf51_adc_start(void)
{
	nrf51_adc_task_trigger(NRF51_ADC_TASK_TRIGGER);
}

void nrf51_adc_stop(void)
{
	nrf51_adc_task_trigger(NRF51_ADC_TASK_TRIGGER);
}

bool nrf51_adc_conversion_finished(void)
{
    return ((bool)NRF_ADC->EVENTS_END);
}

void nrf51_adc_conversion_event_clean(void)
{
	nrf51_adc_event_clean(NRF51_ADC_EVENT_CLEAR);
}

uint32_t *nrf51_adc_task_address_get(enum nrf51_adc_task_e task)
{
    return (uint32_t *)((uint8_t *)NRF_ADC + task);
}

uint32_t *nrf51_adc_event_address_get(enum nrf51_adc_event_e event)
{
    return (uint32_t *)((uint8_t *)NRF_ADC + event);
}

void nrf51_adc_task_trigger(enum nrf51_adc_task_e task)
{
	*(volatile uint32_t *)((uint8_t *)NRF_ADC + task) = NRF51_ADC_TASK_TRIGGER;
}

void nrf51_adc_event_clean(enum nrf51_adc_event_e event)
{
	*(volatile uint32_t *)((uint8_t *)NRF_ADC + event) = NRF51_ADC_EVENT_CLEAR;
}
