#ifndef _GATTS_HANDLER_
#define _GATTS_HANDLER_

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
//#include "nrf_gpio.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
//#include "ble_advertising.h"
//#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "bsp.h"
//#include "bsp_btn_ble.h"

#include <stdarg.h>
#include <stdbool.h>

#include "peer_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*fp_rx_handler_t)(uint8_t *p_data, unsigned length);

typedef void (*fp_tx_handler_t)(bool success);

struct ble_trs_s {
	uint16_t service_handle;
	ble_gatts_char_handles_t tx_char_handles;
	ble_gatts_char_handles_t rx_char_handles;

	fp_rx_handler_t fp_rx_handler;
	fp_tx_handler_t fp_tx_handler;

	bool isHVXEnabled;
};

#define BLE_UUID_BASE_UUID                 {{0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}} // 128-bit base UUID
#define BLE_UUID_HOI_SERVICE               0xABCD // Just a random, but recognizable value
#define BLE_UUID_TX_CHARACTERISTC_UUID     0x1234
#define BLE_UUID_RX_CHARACTERISTC_UUID     0x5678

#define MAX_VALUE_BUF_LEN                   20


int trs_init(fp_rx_handler_t fp_rx_handler, fp_tx_handler_t fp_tx_handler);
int trs_service_add(void);

int trs_tx_char_add(void);
int trs_rx_char_add(void);

void gatts_handler(ble_evt_t *p_evt);

int ble_trs_transmit(uint8_t *data, unsigned length);

#ifdef __cplusplus
}
#endif

#endif
