#include "nrf51_uart.h"

#include <stdint.h>

#define NRF51_UART_PSEL_DISCONNECTED		0xFFFFFFFF




void nrf51_uart_cfg(NRF_UART_Type *reg, enum nrf51_uart_baudrate_e b, enum nrf51_uart_parity_e p, enum nrf51_uart_hwfc_e h)
{
	reg->CONFIG = (uint32_t)p | (uint32_t)h;
	reg->BAUDRATE = b;
}

void nrf51_uart_rxtx_pin(NRF_UART_Type *reg, uint32_t rxpin, uint32_t txpin)
{
	reg->PSELTXD = txpin;
    reg->PSELRXD = rxpin;
}

void nrf51_uart_rtscts_pin(NRF_UART_Type *reg, uint32_t rtspin, uint32_t ctspin)
{
	reg->PSELRTS = rtspin;
    reg->PSELCTS = ctspin;
}

void nrf51_uart_disconnect_rxtx_pin(NRF_UART_Type *reg)
{
	reg->PSELTXD = NRF51_UART_PSEL_DISCONNECTED;
    reg->PSELRXD = NRF51_UART_PSEL_DISCONNECTED;
}

void nrf51_uart_disconnect_rtscts_pin(NRF_UART_Type *reg)
{
	reg->PSELRTS = NRF51_UART_PSEL_DISCONNECTED;
    reg->PSELCTS = NRF51_UART_PSEL_DISCONNECTED;
}

void nrf51_uart_enable(NRF_UART_Type *reg)
{
	reg->ENABLE = UART_ENABLE_ENABLE_Enabled;
}

void nrf51_uart_disable(NRF_UART_Type *reg)
{
	reg->ENABLE = UART_ENABLE_ENABLE_Disabled;
}

void nrf51_uart_rx_enable(NRF_UART_Type *reg)
{
	nrf51_uart_task_trigger(reg, NRF51_UART_TASK_STARTRX);
}

void nrf51_uart_tx_enable(NRF_UART_Type *reg)
{
	nrf51_uart_task_trigger(reg, NRF51_UART_TASK_STARTTX);
}


void nrf51_uart_tx_disable(NRF_UART_Type *reg)
{
	nrf51_uart_task_trigger(reg, NRF51_UART_TASK_STOPTX);
}

void nrf51_uart_rx_disable(NRF_UART_Type *reg)
{
	nrf51_uart_task_trigger(reg, NRF51_UART_TASK_STOPRX);
}

uint8_t nrf51_uart_get(NRF_UART_Type *reg)
{
	return (uint8_t)reg->RXD;
}

void nrf51_uart_put(NRF_UART_Type *reg, uint8_t data)
{
	reg->TXD = data;
}

void nrf51_uart_int_set(NRF_UART_Type *reg, uint32_t mask)
{
	reg->INTENSET = mask;
}

void nrf51_uart_int_clr(NRF_UART_Type *reg, uint32_t mask)
{
	reg->INTENCLR = mask;
}

uint32_t nrf51_uart_err_get_clr(NRF_UART_Type *reg)
{
	uint32_t errsrc_mask = reg->ERRORSRC;
    reg->ERRORSRC = errsrc_mask;
    return errsrc_mask;
}

void nrf51_uart_event_clear(NRF_UART_Type * reg, enum nrf51_uart_event_e event)
{
	*((volatile uint32_t *)((uint8_t *)reg + (uint32_t)event)) = 0x0UL;
}

int nrf51_uart_event_check(NRF_UART_Type *reg, enum nrf51_uart_event_e event)
{
	return !!(*(volatile uint32_t *)((uint8_t *)reg + (uint32_t)event));
}

void nrf51_uart_task_trigger(NRF_UART_Type *reg, enum nrf51_uart_task_e task)
{
    *((volatile uint32_t *)((uint8_t *)reg + (uint32_t)task)) = 0x1UL;
}
