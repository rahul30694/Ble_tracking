#include "uart.h"

#include "nrf51/nrf51_uart.h"
#include "bsp.h"
#include "nrf_drv_common.h"
#include "circularBuffer.h"

#include <stdio.h>



CIRCBUF_DEF(uint8_t, rx_buf, 128);

int map_baud_rate(uint32_t baud, enum nrf51_uart_baudrate_e *nrf51_bd)
{
	if (baud == 1200)
		*nrf51_bd = NRF51_UART_BAUDRATE_1200;
	else if (baud == 2400)
		*nrf51_bd = NRF51_UART_BAUDRATE_2400;
	else if (baud == 4800)
		*nrf51_bd = NRF51_UART_BAUDRATE_4800;
	else if (baud == 9600)
		*nrf51_bd = NRF51_UART_BAUDRATE_9600;
	else if (baud == 14400)
		*nrf51_bd = NRF51_UART_BAUDRATE_14400;
	else if (baud == 19200)
		*nrf51_bd = NRF51_UART_BAUDRATE_19200;
	else if (baud == 28800)
		*nrf51_bd = NRF51_UART_BAUDRATE_28800;
	else if (baud == 38400)
		*nrf51_bd = NRF51_UART_BAUDRATE_38400;
	else if (baud == 57600)
		*nrf51_bd = NRF51_UART_BAUDRATE_57600;
	else if (baud == 76800)
		*nrf51_bd = NRF51_UART_BAUDRATE_76800;
	else if (baud == 115200)
		*nrf51_bd = NRF51_UART_BAUDRATE_115200;
	else if (baud == 230400)
		*nrf51_bd = NRF51_UART_BAUDRATE_230400;
	else if (baud == 250000)
		*nrf51_bd = NRF51_UART_BAUDRATE_250000;
	else if (baud == 460800)
		*nrf51_bd = NRF51_UART_BAUDRATE_460800;
	else if (baud == 921600)
		*nrf51_bd = NRF51_UART_BAUDRATE_921600;
	else if (baud == 1000000)
		*nrf51_bd = NRF51_UART_BAUDRATE_1000000;
	else
		return -1;
	return 0;
}

int uart_cfg(uint32_t baud, bool parity)
{
	enum nrf51_uart_baudrate_e nrf51_bd;
	enum nrf51_uart_parity_e nrf51_parity = NRF51_UART_PARITY_EXCLUDED;

	if (map_baud_rate(baud, &nrf51_bd))
		return -1;
	if (parity)
		nrf51_parity = NRF51_UART_PARITY_INCLUDED;

	nrf51_uart_cfg(NRF_UART0, nrf51_bd, nrf51_parity, NRF51_UART_HWFC_DISABLED);

	nrf51_uart_rxtx_pin(NRF_UART0, RX_PIN_NUMBER, TX_PIN_NUMBER);
	nrf51_uart_disconnect_rtscts_pin(NRF_UART0);

	nrf51_uart_event_clear(NRF_UART0, NRF51_UART_EVENT_ERROR);
	nrf51_uart_event_clear(NRF_UART0, NRF51_UART_EVENT_RXDRDY);

	nrf51_uart_int_set(NRF_UART0, NRF51_UART_INT_MASK_RXDRDY);
	nrf51_uart_int_clr(NRF_UART0, NRF51_UART_INT_MASK_TXDRDY | NRF51_UART_INT_MASK_CTS |
			NRF51_UART_INT_MASK_ERROR | NRF51_UART_INT_MASK_NCTS | NRF51_UART_INT_MASK_RXTO);

	nrf_drv_common_irq_enable(UART0_IRQn, 3);
	nrf51_uart_enable(NRF_UART0);

	nrf51_uart_tx_disable(NRF_UART0);
	nrf51_uart_rx_enable(NRF_UART0);

	return 0;
}

uint8_t uart_getchar_unlocked(void)
{
	uint8_t data;
	rx_buf.deque(&data);
	return data;
}

int uart_putchar_unlocked(uint8_t data)
{
	nrf51_uart_tx_enable(NRF_UART0);
	nrf51_uart_event_clear(NRF_UART0, NRF51_UART_EVENT_TXDRDY);
	nrf51_uart_put(NRF_UART0, data);
	while (!nrf51_uart_event_check(NRF_UART0, NRF51_UART_EVENT_TXDRDY));
	nrf51_uart_tx_disable(NRF_UART0);
	return 0;
}

bool uart_is_available(void)
{
	return !rx_buf.isEmpty();
}

int uart_deinit(void)
{
	nrf51_uart_tx_disable(NRF_UART0);
	nrf51_uart_rx_disable(NRF_UART0);
	nrf51_uart_disable(NRF_UART0);
	return 0;

}

#if defined(__GNUC__)
extern "C" {

int _write(int file, const char *buf, int len)
{
	if (file != 1)
		return -1;
    int i;
    for (i = 0; i < len; i++)
    	uart_putchar_unlocked(*buf++);

    return len;
}


int _read(int file, char *buf, int len)
{
    while (!uart_is_available());
    *buf = uart_getchar_unlocked();
    return 1;
}

}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void UART0_IRQHandler(void)
{
	if (nrf51_uart_event_check(NRF_UART0, NRF51_UART_EVENT_RXDRDY)) {
		nrf51_uart_event_clear(NRF_UART0, NRF51_UART_EVENT_RXDRDY);
		uint8_t data = nrf51_uart_get(NRF_UART0);
		rx_buf.enque(&data);
	}
}

#ifdef __cplusplus
}
#endif
