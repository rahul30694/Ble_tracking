#include "conn_manager.h"

#include "sdk_common.h"
#include "ble.h"
#include "ble_gap.h"

#include <stdint.h>
#include <stdbool.h>

void snprint_ble_addr(char *buf, unsigned buf_len, ble_gap_addr_t *addr)
{
	snprintf(buf, buf_len, "%02x:%02x:%02x:%02x:%02x:%02x", addr->addr[5], addr->addr[4], addr->addr[3],
									addr->addr[2], addr->addr[1], addr->addr[0]);
}

void snprint_master_id(char *buf, unsigned buf_len, ble_gap_master_id_t *master_id)
{
	snprintf(buf, buf_len, "EDIV: %u, RAND: %u.%u.%u.%u.%u.%u.%u.%u", master_id->ediv, master_id->rand[0], 
						master_id->rand[1], master_id->rand[2], master_id->rand[3], master_id->rand[4],
						master_id->rand[5], master_id->rand[6], master_id->rand[7]);
}

void snprint_passkey(char *buf, unsigned buf_len, ble_gap_evt_passkey_display_t *evt)
{
	snprintf(buf, buf_len, "%02x, %02x, %02x, %02x, %02x, %02x", evt->passkey[0], evt->passkey[1],
												evt->passkey[2], evt->passkey[3], evt->passkey[4], 
												evt->passkey[5]);
}

void snprint_conn_params(char *buf, unsigned buf_len, ble_gap_conn_params_t *evt)
{
	snprintf(buf, buf_len, "Min Conn Int(1.25ms): %d Max Conn Int(1.25ms): %d Slave lat: %d Sup To(10ms): %d", evt->min_conn_interval, 
		evt->max_conn_interval, evt->slave_latency, evt->conn_sup_timeout);
}

void print_gap_evt_connected(ble_gap_evt_connected_t *evt)
{
	printf("[Connected Evt]=> ");
	char ble_addr_buf[MAX_BLE_ADDR_CSTR_LEN];

	ble_gap_addr_t *peer_addr = &(evt->peer_addr);
	ble_gap_addr_t *own_addr = &(evt->own_addr);
	ble_gap_conn_params_t *conn_params = &(evt->conn_params);


    snprint_ble_addr(ble_addr_buf, MAX_BLE_ADDR_CSTR_LEN, peer_addr);
    printf("BLE Addr: %s, ", ble_addr_buf);

    snprint_ble_addr(ble_addr_buf, MAX_BLE_ADDR_CSTR_LEN, own_addr);
    printf("Own Addr: %s, ", ble_addr_buf);

    if (evt->irk_match)
    	printf("IRK Match with index - %u", evt->irk_match_idx);
    printf("\r\n");

    char ble_conn_params_buf[MAX_BLE_CONN_PARAM_PRINT_LEN];
    snprint_conn_params(ble_conn_params_buf, sizeof(ble_conn_params_buf), conn_params);

    printf("[..Connected Evt]=> Con Param:	 %s\r\n", ble_conn_params_buf);

}

void print_gap_evt_disconnected(ble_gap_evt_disconnected_t *evt)
{
	printf("[Disconnected Evt]=> Reason: %d\r\n", evt->reason);
}

void print_gap_evt_conn_param_update(ble_gap_evt_conn_param_update_t *evt)
{
	ble_gap_conn_params_t *param = &(evt->conn_params);

	char buf[MAX_BLE_CONN_PARAM_PRINT_LEN];
	snprint_conn_params(buf, sizeof(buf), param);
	printf("[Conn Param Update]=> Param: %s\r\n", buf);
}

void print_gap_evt_sec_params(ble_gap_evt_sec_params_request_t *evt)
{
	ble_gap_sec_params_t *p = &(evt->peer_params);

	printf("[S Params Req]=> Min K-(%d) Max K-(%d) ", p->min_key_size, p->max_key_size);
	if (p->bond)
		printf("| BOND |");
	if (p->mitm)
		printf("| MITM |");
	if (p->oob)
		printf("| OOB |");

	printf(" IO Caps: ");
	if (p->io_caps == BLE_GAP_IO_CAPS_DISPLAY_ONLY)
		printf("DISPLAY_ONLY");
	else if (p->io_caps == BLE_GAP_IO_CAPS_DISPLAY_YESNO)
		printf("DISPLAY_YESNO");
	else if (p->io_caps == BLE_GAP_IO_CAPS_KEYBOARD_ONLY)
		printf("KEYBOARD_ONLY");
	//else if (p->io_caps == BLE_GAP_IO_CAPS_NONE)
		//printf("NONE");
	else if (p->io_caps == BLE_GAP_IO_CAPS_KEYBOARD_DISPLAY)
		printf("KEYBOARD_DISPLAY");
	else
		printf("NONE");

	printf("\r\n");

	printf("[S Params Req]=> ");

	ble_gap_sec_kdist_t *central = &(p->kdist_central);
	printf("Central Key: Enc-%d Id-%d Sign-%d", central->enc, central->id, central->sign);

	ble_gap_sec_kdist_t *peri = &(p->kdist_periph);
	printf("Peri Key: Enc-%d Id-%d Sign-%d\r\n", peri->enc, peri->id, peri->sign);

}

void print_gap_evt_sec_info_req(ble_gap_evt_sec_info_request_t *evt)
{
	char ble_addr_buf[MAX_BLE_ADDR_CSTR_LEN];

	ble_gap_addr_t *addr = &(evt->peer_addr);

    snprint_ble_addr(ble_addr_buf, MAX_BLE_ADDR_CSTR_LEN, addr);
	

	char ble_master_id[MAX_BLE_MASTER_ID_LEN];
	snprint_master_id(ble_master_id, MAX_BLE_MASTER_ID_LEN, &(evt->master_id));
	printf("[Sec Info Req]=> Peer: %s, Master ID: %s and Required info:	", ble_addr_buf, ble_master_id);

	if (evt->enc_info)
		printf(" enc_info");
	if (evt->id_info)
		printf(" id_info");
	if (evt->sign_info)
		printf(" sign_info");
	printf("\r\n");

}

void print_gap_evt_auth_key_request(ble_gap_evt_auth_key_request_t *evt)
{
	printf("[Auth Key Request]=> Type: ");

	uint8_t key_type = evt->key_type;
	if (key_type == BLE_GAP_AUTH_KEY_TYPE_NONE)
		printf("None");
	else if (key_type == BLE_GAP_AUTH_KEY_TYPE_PASSKEY)
		printf("Passkey");
	else
		printf("OOB");
	printf("\r\n");
}

void print_gap_evt_auth_status(ble_gap_evt_auth_status_t *evt)
{
	printf("[Auth Status Evt]=> Result: ");
	uint8_t auth_status = evt->auth_status;
	if (auth_status == BLE_GAP_SEC_STATUS_SUCCESS)
		printf("Success");
	else if (auth_status == BLE_GAP_SEC_STATUS_TIMEOUT)
		printf("Timeout");
	else if (auth_status == BLE_GAP_SEC_STATUS_PDU_INVALID)
		printf("PDU Invalid");
	else if (auth_status == BLE_GAP_SEC_STATUS_PASSKEY_ENTRY_FAILED)
		printf("Passkey Failed");
	else if (auth_status == BLE_GAP_SEC_STATUS_OOB_NOT_AVAILABLE)
		printf("OOB Not Available");
	else if (auth_status == BLE_GAP_SEC_STATUS_AUTH_REQ)
		printf("Auth Req not met");
	else if (auth_status == BLE_GAP_SEC_STATUS_CONFIRM_VALUE)
		printf("Confirm value failed");
	else if (auth_status == BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP)
		printf("Pairing not supported");
	else if (auth_status == BLE_GAP_SEC_STATUS_ENC_KEY_SIZE)
		printf("Encryption key size");
	else if (auth_status == BLE_GAP_SEC_STATUS_SMP_CMD_UNSUPPORTED)
		printf("Unsupported SMP command");
	else if (auth_status == BLE_GAP_SEC_STATUS_UNSPECIFIED)
		printf("Unspecified reason");
	else if (auth_status == BLE_GAP_SEC_STATUS_REPEATED_ATTEMPTS)
		printf("Too little time elapsed since last attempt.");
	else if (auth_status == BLE_GAP_SEC_STATUS_INVALID_PARAMS)
		printf("Invalid parameters");
	else
		printf("Unknown");

	if (evt->bonded)
		printf(" Successfull Bonding");

	ble_gap_sec_levels_t *sm1_levels = &(evt->sm1_levels);
	ble_gap_sec_levels_t *sm2_levels = &(evt->sm2_levels);
	printf(" Max Sec Level M1- %u M2-%u", sm1_levels->lv1 ? 1 : (sm1_levels->lv2 ? 2 : (sm1_levels->lv3 ? 3 : 0)), 
										sm2_levels->lv1 ? 1 : (sm2_levels->lv2 ? 2 : (sm2_levels->lv3 ? 3 : 0)));
	printf("\r\n");
	printf("[...Auth Status Evt]=> ");
	ble_gap_sec_kdist_t *kdist_periph = &(evt->kdist_periph);
	ble_gap_sec_kdist_t *kdist_central = &(evt->kdist_central);
	
	printf("Keys for Peri Enc:%u Id:%u Sign:%u", kdist_periph->enc, kdist_periph->id, kdist_periph->sign);
	printf("Keys for Central Enc:%u Id:%u Sign:%u\r\n", kdist_central->enc, kdist_central->id, kdist_central->sign);
 
}

void print_gap_evt_sec_update(ble_gap_evt_conn_sec_update_t *evt)
{
	ble_gap_conn_sec_t *sec = &(evt->conn_sec);
	ble_gap_conn_sec_mode_t *mode = &(sec->sec_mode);

	uint8_t encr_key_size = sec->encr_key_size;

	printf("[Conn Sec Param Update]=> Request with Key Size-%u and Mode-%u Level-%u\r\n", encr_key_size, mode->sm, mode->lv);
}


void print_gap_evt_timeout(ble_gap_evt_timeout_t *evt)
{
	printf("[Timeout Evt]=> Source: ");
	if (evt->src == BLE_GAP_TIMEOUT_SRC_ADVERTISING)
		printf("Advertising");
	else if (evt->src == BLE_GAP_TIMEOUT_SRC_SECURITY_REQUEST)
		printf("Security Req");
	else if (evt->src == BLE_GAP_TIMEOUT_SRC_SCAN)
		printf("Scan");
	else if (evt->src == BLE_GAP_TIMEOUT_SRC_CONN)
		printf("Connection");
	else
		printf("None");
	printf("\r\n");
}

void print_adv_report(ble_gap_evt_adv_report_t *evt)
{
	ble_gap_addr_t *addr = &(evt->peer_addr);

	char ble_addr_buf[MAX_BLE_ADDR_CSTR_LEN];
	snprint_ble_addr(ble_addr_buf, MAX_BLE_ADDR_CSTR_LEN, addr);

	printf("[Adv Report]=> ");
	if (evt->scan_rsp)
		printf("[Scan Response] ");
	else
		printf("[Adv Report] ");

	printf(" Peer %s, RSSI %d ", ble_addr_buf, evt->rssi);

	if (!evt->scan_rsp) {
		printf(" Adv Type:");
		if (evt->type == BLE_GAP_ADV_TYPE_ADV_IND)
			printf("ADV IND");
		else if (evt->type == BLE_GAP_ADV_TYPE_ADV_DIRECT_IND)
			printf("Direct Ind");
		else if (evt->type == BLE_GAP_ADV_TYPE_ADV_SCAN_IND)
			printf("Adv Scan Ind");
		else
			printf("Non Conn Ind");
	}
	printf("\r\n");
}