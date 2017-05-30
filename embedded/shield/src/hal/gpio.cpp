#include "gpio.h"

#include "nrf51/nrf51_gpio.h"

void gpio_input_cfg(uint32_t pin, enum gpio_pin_pull_e pull)
{
	enum nrf51_gpio_pin_pull_e nrf51_pull = NRF51_GPIO_PIN_NOPULL;

	if (pull == GPIO_PULL_UP)
		nrf51_pull = NRF51_GPIO_PIN_PULL_UP;
	else if (pull == GPIO_PULL_DOWN)
		nrf51_pull = NRF51_GPIO_PIN_PULL_DOWN;

	nrf51_gpio_cfg(pin, NRF51_GPIO_PIN_DIR_INPUT,
		NRF51_GPIO_PIN_INPUT_CONNECT,
		nrf51_pull,
		NRF51_GPIO_PIN_S0S1,
		NRF51_GPIO_PIN_NOSENSE);
}

void gpio_output_cfg(uint32_t pin)
{
	nrf51_gpio_cfg(pin, NRF51_GPIO_PIN_DIR_OUTPUT,
		NRF51_GPIO_PIN_INPUT_DISCONNECT,
		NRF51_GPIO_PIN_NOPULL,
		NRF51_GPIO_PIN_S0S1,
		NRF51_GPIO_PIN_NOSENSE);
}


void gpio_write(uint32_t pin, bool value)
{
	if (value)
		nrf51_gpio_pin_set(pin);
	else
		nrf51_gpio_pin_clear(pin);
}

bool gpio_read(uint32_t pin)
{
	return (bool)nrf51_gpio_pin_read(pin);
}