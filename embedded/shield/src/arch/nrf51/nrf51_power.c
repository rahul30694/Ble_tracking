#include "nrf51_power.h"


void nrf51_power_int_mask_set(enum nrf51_power_int_mask_e mask)
{
	NRF_POWER->INTENSET = mask;
}

void nrf51_power_int_mask_clear(enum nrf51_power_int_mask_e mask)
{
	NRF_POWER->INTENCLR = mask;
}

bool nrf51_power_rst_reason_check(enum nrf51_power_rst_reasons_e reason)
{
	return !!(NRF_POWER->RESETREAS & reason);
}

void nrf51_power_rst_reason_clear(enum nrf51_power_rst_reasons_e reason)
{
	NRF_POWER->RESETREAS &= ~reason;
}

bool nrf51_power_ram_check(enum nrf51_power_ramstatus_e ram)
{
	return !!(NRF_POWER->RAMSTATUS & ram);
}

void nrf51_power_off_mode(void)
{
	NRF_POWER->SYSTEMOFF = POWER_SYSTEMOFF_SYSTEMOFF_Enter << POWER_SYSTEMOFF_SYSTEMOFF_Pos;
}

void nrf51_power_failure_mode(enum nrf51_power_failure_e pf)
{
	NRF_POWER->POFCON = (uint32_t)pf;
}

void nrf51_power_set_retained_register(uint8_t value)
{
	NRF_POWER->GPREGRET = value;
}

uint8_t nrf51_power_get_retained_register(void)
{
	return (uint8_t)(NRF_POWER->GPREGRET);
}

void nrf51_power_en_rst_dif(void)
{
	NRF_POWER->RESET = POWER_RESET_RESET_Enabled << POWER_RESET_RESET_Pos;
}

void nrf51_power_disable_rst_dif(void)
{
	NRF_POWER->RESET = POWER_RESET_RESET_Disabled << POWER_RESET_RESET_Pos;
}

void nrf51_power_en_dcdc(void)
{
	NRF_POWER->DCDCEN = POWER_DCDCEN_DCDCEN_Enabled << POWER_DCDCEN_DCDCEN_Pos;
}

void nrf51_power_disable_dcdc(void)
{
	NRF_POWER->DCDCEN = POWER_DCDCEN_DCDCEN_Disabled << POWER_DCDCEN_DCDCEN_Pos;
}

void nrf51_power_onram(uint32_t ram_mask)
{
	if (ram_mask > NRF51_POWER_ONRAM_OFF_1)
		NRF_POWER->RAMONB = ram_mask;
	else
		NRF_POWER->RAMON = ram_mask;
}

void nrf51_power_offram(uint32_t ram_mask)
{
	if (ram_mask > NRF51_POWER_OFFRAM_OFF_1)
		NRF_POWER->RAMONB = ram_mask;
	else
		NRF_POWER->RAMON = ram_mask;
}
