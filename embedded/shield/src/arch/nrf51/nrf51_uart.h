#ifndef _NRF51_UART_H_
#define _NRF51_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

enum nrf51_uart_task_e {
    NRF51_UART_TASK_STARTRX = offsetof(NRF_UART_Type, TASKS_STARTRX),
    NRF51_UART_TASK_STOPRX  = offsetof(NRF_UART_Type, TASKS_STOPRX),
    NRF51_UART_TASK_STARTTX = offsetof(NRF_UART_Type, TASKS_STARTTX),
    NRF51_UART_TASK_STOPTX  = offsetof(NRF_UART_Type, TASKS_STOPTX),
    NRF51_UART_TASK_SUSPEND = offsetof(NRF_UART_Type, TASKS_SUSPEND),
};

enum nrf51_uart_event_e {
    NRF51_UART_EVENT_CTS    = offsetof(NRF_UART_Type, EVENTS_CTS),
    NRF51_UART_EVENT_NCTS   = offsetof(NRF_UART_Type, EVENTS_NCTS),
    NRF51_UART_EVENT_RXDRDY = offsetof(NRF_UART_Type, EVENTS_RXDRDY),
    NRF51_UART_EVENT_TXDRDY = offsetof(NRF_UART_Type, EVENTS_TXDRDY),
    NRF51_UART_EVENT_ERROR  = offsetof(NRF_UART_Type, EVENTS_ERROR),
    NRF51_UART_EVENT_RXTO   = offsetof(NRF_UART_Type, EVENTS_RXTO),
};


enum nrf51_uart_int_mask_e {
    NRF51_UART_INT_MASK_CTS    = UART_INTENCLR_CTS_Msk,
    NRF51_UART_INT_MASK_NCTS   = UART_INTENCLR_NCTS_Msk,
    NRF51_UART_INT_MASK_RXDRDY = UART_INTENCLR_RXDRDY_Msk,
    NRF51_UART_INT_MASK_TXDRDY = UART_INTENCLR_TXDRDY_Msk,
    NRF51_UART_INT_MASK_ERROR  = UART_INTENCLR_ERROR_Msk,
    NRF51_UART_INT_MASK_RXTO   = UART_INTENCLR_RXTO_Msk,
};


enum nrf51_uart_baudrate_e {
    NRF51_UART_BAUDRATE_1200   	=  UART_BAUDRATE_BAUDRATE_Baud1200, /**< 1200 baud. */
    NRF51_UART_BAUDRATE_2400   	=  UART_BAUDRATE_BAUDRATE_Baud2400, /**< 2400 baud. */
    NRF51_UART_BAUDRATE_4800   	=  UART_BAUDRATE_BAUDRATE_Baud4800, /**< 4800 baud. */
    NRF51_UART_BAUDRATE_9600   	=  UART_BAUDRATE_BAUDRATE_Baud9600, /**< 9600 baud. */
    NRF51_UART_BAUDRATE_14400  	=  UART_BAUDRATE_BAUDRATE_Baud14400, /**< 14400 baud. */
    NRF51_UART_BAUDRATE_19200  	=  UART_BAUDRATE_BAUDRATE_Baud19200, /**< 19200 baud. */
    NRF51_UART_BAUDRATE_28800  	=  UART_BAUDRATE_BAUDRATE_Baud28800, /**< 28800 baud. */
    NRF51_UART_BAUDRATE_38400  	=  UART_BAUDRATE_BAUDRATE_Baud38400, /**< 38400 baud. */
    NRF51_UART_BAUDRATE_57600  	=  UART_BAUDRATE_BAUDRATE_Baud57600, /**< 57600 baud. */
    NRF51_UART_BAUDRATE_76800  	=  UART_BAUDRATE_BAUDRATE_Baud76800, /**< 76800 baud. */
    NRF51_UART_BAUDRATE_115200 	=  UART_BAUDRATE_BAUDRATE_Baud115200, /**< 115200 baud. */
    NRF51_UART_BAUDRATE_230400 	=  UART_BAUDRATE_BAUDRATE_Baud230400, /**< 230400 baud. */
    NRF51_UART_BAUDRATE_250000 	=  UART_BAUDRATE_BAUDRATE_Baud250000, /**< 250000 baud. */
    NRF51_UART_BAUDRATE_460800 	=  UART_BAUDRATE_BAUDRATE_Baud460800, /**< 460800 baud. */
    NRF51_UART_BAUDRATE_921600 	=  UART_BAUDRATE_BAUDRATE_Baud921600, /**< 921600 baud. */
    NRF51_UART_BAUDRATE_1000000 =  UART_BAUDRATE_BAUDRATE_Baud1M, /**< 1000000 baud. */
};

enum nrf51_uart_error_mask_e {
    NRF51_UART_ERROR_OVERRUN_MASK = UART_ERRORSRC_OVERRUN_Msk,
    NRF51_UART_ERROR_PARITY_MASK  = UART_ERRORSRC_PARITY_Msk,
    NRF51_UART_ERROR_FRAMING_MASK = UART_ERRORSRC_FRAMING_Msk,
    NRF51_UART_ERROR_BREAK_MASK   = UART_ERRORSRC_BREAK_Msk,	
};

enum nrf51_uart_parity_e {
    NRF51_UART_PARITY_EXCLUDED = UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos,
    NRF51_UART_PARITY_INCLUDED = UART_CONFIG_PARITY_Included << UART_CONFIG_PARITY_Pos,
};


enum nrf51_uart_hwfc_e {
    NRF51_UART_HWFC_DISABLED = UART_CONFIG_HWFC_Disabled,
    NRF51_UART_HWFC_ENABLED  = UART_CONFIG_HWFC_Enabled,
};



void nrf51_uart_cfg(NRF_UART_Type *reg, enum nrf51_uart_baudrate_e b, enum nrf51_uart_parity_e p, enum nrf51_uart_hwfc_e h);

void nrf51_uart_rxtx_pin(NRF_UART_Type *reg, uint32_t rxpin, uint32_t txpin);

void nrf51_uart_rtscts_pin(NRF_UART_Type *reg, uint32_t rtspin, uint32_t ctspin);

void nrf51_uart_disconnect_rxtx_pin(NRF_UART_Type *reg);

void nrf51_uart_disconnect_rtscts_pin(NRF_UART_Type *reg);

void nrf51_uart_enable(NRF_UART_Type *reg);
void nrf51_uart_disable(NRF_UART_Type *reg);

void nrf51_uart_rx_enable(NRF_UART_Type *reg);
void nrf51_uart_tx_enable(NRF_UART_Type *reg);


void nrf51_uart_tx_disable(NRF_UART_Type *reg);

void nrf51_uart_rx_disable(NRF_UART_Type *reg);

uint8_t nrf51_uart_get(NRF_UART_Type *reg);

void nrf51_uart_put(NRF_UART_Type *reg, uint8_t data);

void nrf51_uart_int_set(NRF_UART_Type *reg, uint32_t mask);

void nrf51_uart_int_clr(NRF_UART_Type *reg, uint32_t mask);

uint32_t nrf51_uart_err_get_clr(NRF_UART_Type *reg);

void nrf51_uart_event_clear(NRF_UART_Type * reg, enum nrf51_uart_event_e event);

int nrf51_uart_event_check(NRF_UART_Type *reg, enum nrf51_uart_event_e event);

void nrf51_uart_task_trigger(NRF_UART_Type *reg, enum nrf51_uart_task_e task);

#ifdef __cplusplus
}
#endif


#endif
