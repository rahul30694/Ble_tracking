#include "nrf51_gpio.h"


void nrf51_gpio_cfg(uint32_t pin, 
	enum nrf51_pin_dir_e dir, 
	enum nrf51_gpio_pin_input_e input, 
	enum nrf51_gpio_pin_pull_e pull,
	enum nrf51_gpio_pin_drive_e drive, 
	enum nrf51_gpio_pin_sense_e sense)
{
	NRF_GPIO->PIN_CNF[pin] = ((uint32_t)dir   << GPIO_PIN_CNF_DIR_Pos)
                                  | ((uint32_t)input << GPIO_PIN_CNF_INPUT_Pos)
                                  | ((uint32_t)pull  << GPIO_PIN_CNF_PULL_Pos)
                                  | ((uint32_t)drive << GPIO_PIN_CNF_DRIVE_Pos)
                                  | ((uint32_t)sense << GPIO_PIN_CNF_SENSE_Pos);
}

void nrf51_gpio_cfg_input(uint32_t pin)
{
	nrf51_gpio_cfg(pin, NRF51_GPIO_PIN_DIR_INPUT,
		NRF51_GPIO_PIN_INPUT_CONNECT,
		NRF51_GPIO_PIN_NOPULL,
		NRF51_GPIO_PIN_S0S1,
		NRF51_GPIO_PIN_NOSENSE);
}


void nrf51_gpio_cfg_output(uint32_t pin)
{
	nrf51_gpio_cfg(pin, NRF51_GPIO_PIN_DIR_OUTPUT,
		NRF51_GPIO_PIN_INPUT_DISCONNECT,
		NRF51_GPIO_PIN_NOPULL,
		NRF51_GPIO_PIN_S0S1,
		NRF51_GPIO_PIN_NOSENSE);
}

void nrf51_gpio_pin_set(uint32_t pin)
{
	NRF_GPIO->OUTSET = (1UL << pin);
}

void nrf51_gpio_pin_clear(uint32_t pin)
{
	NRF_GPIO->OUTCLR = (1UL << pin);
}

int nrf51_gpio_pin_read(uint32_t pin)
{
	return (!!(NRF_GPIO->IN & (1UL << pin)));
}


void nrf51_gpio_cfg_input_connect(uint32_t pin)
{
    uint32_t cnf = NRF_GPIO->PIN_CNF[pin] & ~GPIO_PIN_CNF_INPUT_Msk;
    NRF_GPIO->PIN_CNF[pin] = cnf | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos);
}

void nrf51_gpio_cfg_input_disconnect(uint32_t pin)
{
    uint32_t cnf = NRF_GPIO->PIN_CNF[pin] & ~GPIO_PIN_CNF_INPUT_Msk;
    NRF_GPIO->PIN_CNF[pin] = cnf | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos);
}

void nrf51_gpio_cfg_input_sense(uint32_t pin, enum nrf51_gpio_pin_sense_e sense)
{
    NRF_GPIO->PIN_CNF[pin] &= ~GPIO_PIN_CNF_SENSE_Msk;
    NRF_GPIO->PIN_CNF[pin] |= ((uint32_t)sense << GPIO_PIN_CNF_SENSE_Pos);
}


void nrf51_gpio_pins_set(uint32_t pin_mask)
{
    NRF_GPIO->OUTSET = pin_mask;
}

void nrf51_gpio_pins_clear(uint32_t pin_mask)
{
    NRF_GPIO->OUTCLR = pin_mask;
}

uint32_t nrf51_gpio_pins_read(void)
{
    return NRF_GPIO->IN;
}
