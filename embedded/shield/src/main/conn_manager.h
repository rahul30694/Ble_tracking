#ifndef _CONN_MANAGER_H_
#define _CONN_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>
#include "sdk_common.h"
#include "ble.h"
#include "ble_gap.h"


#define MAX_BLE_ADDR_CSTR_LEN			18
#define MAX_BLE_MASTER_ID_LEN			11
#define MAX_BLE_PASSKEY_LEN				7
#define MAX_BLE_CONN_PARAM_PRINT_LEN	150

#define SEC_PARAM_BOND                  0                  
#define SEC_PARAM_MITM                  1                   
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE
#define SEC_PARAM_OOB                   0                   
#define SEC_PARAM_MIN_KEY_SIZE          7                   
#define SEC_PARAM_MAX_KEY_SIZE          16                  



void snprint_ble_addr(char *buf, unsigned buf_len, ble_gap_addr_t *addr);
void snprint_master_id(char *buf, unsigned buf_len, ble_gap_master_id_t *master_id);
void snprint_passkey(char *buf, unsigned buf_len, ble_gap_evt_passkey_display_t *evt);
void snprint_conn_params(char *buf, unsigned buf_len, ble_gap_conn_params_t *evt);

void print_gap_evt_connected(ble_gap_evt_connected_t *evt);
void print_gap_evt_disconnected(ble_gap_evt_disconnected_t *evt);
void print_gap_evt_conn_param_update(ble_gap_evt_conn_param_update_t *evt);
void print_gap_evt_sec_params(ble_gap_evt_sec_params_request_t *evt);
void print_gap_evt_sec_info_req(ble_gap_evt_sec_info_request_t *evt);
void print_gap_evt_auth_key_request(ble_gap_evt_auth_key_request_t *evt);
void print_gap_evt_auth_status(ble_gap_evt_auth_status_t *evt);
void print_gap_evt_sec_update(ble_gap_evt_conn_sec_update_t *evt);
void print_gap_evt_timeout(ble_gap_evt_timeout_t *evt);
void print_adv_report(ble_gap_evt_adv_report_t *evt);


#endif