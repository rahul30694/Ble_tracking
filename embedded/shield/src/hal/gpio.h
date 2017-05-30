#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>
#include <stdbool.h>

enum gpio_pin_pull_e {
	GPIO_NOPULL,
	GPIO_PULL_UP,
	GPIO_PULL_DOWN,
};

void gpio_input_cfg(uint32_t pin, enum gpio_pin_pull_e pull);
void gpio_output_cfg(uint32_t pin);

void gpio_write(uint32_t pin, bool value);
bool gpio_read(uint32_t pin);


#endif