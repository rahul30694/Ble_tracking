#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

int uart_cfg(uint32_t baud, bool parity);

uint8_t uart_getchar_unlocked(void);

int uart_putchar_unlocked(uint8_t data);

bool uart_is_available(void);

int uart_deinit(void);
#endif
