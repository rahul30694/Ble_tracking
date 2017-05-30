#ifndef _NRF51_GPIO_H_
#define _NRF51_GPIO_H_


#include "nrf.h"
#include "nrf51.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define NRF51_NUMBER_OF_PINS	32


enum nrf51_pin_dir_e {
	NRF51_GPIO_PIN_DIR_INPUT 	= 	GPIO_PIN_CNF_DIR_Input,
	NRF51_GPIO_PIN_DIR_OUTPUT 	= 	GPIO_PIN_CNF_DIR_Output,
};

enum nrf51_gpio_pin_input_e {
    NRF51_GPIO_PIN_INPUT_CONNECT 	=	GPIO_PIN_CNF_INPUT_Connect,
    NRF51_GPIO_PIN_INPUT_DISCONNECT	=	GPIO_PIN_CNF_INPUT_Disconnect,
};


enum nrf51_gpio_pin_pull_e {
	NRF51_GPIO_PIN_NOPULL		=	GPIO_PIN_CNF_PULL_Disabled,
	NRF51_GPIO_PIN_PULL_UP		=	GPIO_PIN_CNF_PULL_Pullup,
	NRF51_GPIO_PIN_PULL_DOWN	=	GPIO_PIN_CNF_PULL_Pulldown,
};

enum nrf51_gpio_pin_drive_e {
    NRF51_GPIO_PIN_S0S1		=	GPIO_PIN_CNF_DRIVE_S0S1, 	
    NRF51_GPIO_PIN_H0S1		=	GPIO_PIN_CNF_DRIVE_H0S1, 		
    NRF51_GPIO_PIN_S0H1		=	GPIO_PIN_CNF_DRIVE_S0H1, 	
    NRF51_GPIO_PIN_H0H1		=	GPIO_PIN_CNF_DRIVE_H0H1, 
    NRF51_GPIO_PIN_D0S1		=	GPIO_PIN_CNF_DRIVE_D0S1, 
    NRF51_GPIO_PIN_D0H1		=	GPIO_PIN_CNF_DRIVE_D0H1,
    NRF51_GPIO_PIN_S0D1		=	GPIO_PIN_CNF_DRIVE_S0D1,
    NRF51_GPIO_PIN_H0D1		=	GPIO_PIN_CNF_DRIVE_H0D1,
} ;

enum nrf51_gpio_pin_sense_e {
    NRF51_GPIO_PIN_NOSENSE		=	GPIO_PIN_CNF_SENSE_Disabled,
    NRF51_GPIO_PIN_SENSE_LOW	=	GPIO_PIN_CNF_SENSE_Low,
    NRF51_GPIO_PIN_SENSE_HIGH	=	GPIO_PIN_CNF_SENSE_High,
};

void nrf51_gpio_cfg(uint32_t pin, 
    enum nrf51_pin_dir_e dir, 
    enum nrf51_gpio_pin_input_e input, 
    enum nrf51_gpio_pin_pull_e pull,
    enum nrf51_gpio_pin_drive_e drive, 
    enum nrf51_gpio_pin_sense_e sense);


void nrf51_gpio_cfg_input(uint32_t pin);
void nrf51_gpio_cfg_output(uint32_t pin);

void nrf51_gpio_pin_set(uint32_t pin);
void nrf51_gpio_pin_clear(uint32_t pin);

int nrf51_gpio_pin_read(uint32_t pin);

void nrf51_gpio_cfg_input_connect(uint32_t pin);
void nrf51_gpio_cfg_input_disconnect(uint32_t pin);
void nrf51_gpio_cfg_input_sense(uint32_t pin, enum nrf51_gpio_pin_sense_e sense);

void nrf51_gpio_pins_set(uint32_t pin_mask);
void nrf51_gpio_pins_clear(uint32_t pin_mask);

uint32_t nrf51_gpio_pins_read(void);

#ifdef __cplusplus
}
#endif

#endif
