#include "gatts_handler.h"

#include <stdio.h>
#include <string.h>

struct ble_trs_s trs;

static uint8_t tx_value_buf[MAX_VALUE_BUF_LEN];
static uint8_t rx_value_buf[MAX_VALUE_BUF_LEN];

uint8_t m_ble_uuid_index = 0;

int service_add(uint16_t *p_service_handle);
int tx_characteristics_add(uint16_t service_handle, ble_gatts_char_handles_t *p_char_handles);
int rx_characteristics_add(uint16_t service_handle, ble_gatts_char_handles_t *p_char_handles);

uint16_t m_conn_handle = 0xFFFF;


int trs_init(fp_rx_handler_t fp_rx_handler, fp_tx_handler_t fp_tx_handler)
{
	memset(&trs, 0, sizeof(trs));

	trs.fp_rx_handler = fp_rx_handler;
	trs.fp_tx_handler = fp_tx_handler;
	trs.isHVXEnabled = false;

	if (trs_service_add())
        printf("Service Add Failed\r\n");
    if (trs_tx_char_add())
        printf("Failed to add TX characteristic\r\n");
    if (trs_rx_char_add())
        printf("Failed to add RX characteristic\r\n");

	return 0;
}	

int trs_service_add(void)
{
	return service_add(&(trs.service_handle));
}

int trs_tx_char_add(void)
{
	return tx_characteristics_add(trs.service_handle, &trs.tx_char_handles);
}

int trs_rx_char_add(void)
{
	return rx_characteristics_add(trs.service_handle, &trs.rx_char_handles);
}

int trs_send_indication(uint16_t conn_handle)
{
	// return send_notification(uint16_t conn_handle, p_trs->tx_char_handles.value_handle);
	return 0;
}

int service_add(uint16_t *p_service_handle)
{
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid = BLE_UUID_BASE_UUID;

    service_uuid.uuid = BLE_UUID_HOI_SERVICE;

    if (sd_ble_uuid_vs_add(&base_uuid, &m_ble_uuid_index) != NRF_SUCCESS)
        return -1;

    service_uuid.type = m_ble_uuid_index;

    if (sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, p_service_handle)
                        != NRF_SUCCESS)
        return -1;
    return 0;

}


int tx_characteristics_add(uint16_t service_handle, ble_gatts_char_handles_t *p_char_handles)
{
    uint32_t err_code;

    ble_uuid_t char_uuid;
    char_uuid.uuid = BLE_UUID_TX_CHARACTERISTC_UUID;
    char_uuid.type = m_ble_uuid_index;

    // It holds the properties of characteristics which will be present in value of characteristics declaration
    // It also has cccd values
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read = 0;
    char_md.char_props.write = 0;
    char_md.char_props.notify = 0;
    char_md.char_props.indicate = 1;    
    

    // This handles characteristic value declaration attitudes's permissions, loacation to store attritute value and
    // have a flag to set if attribute value is of variable length or not!
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_USER; // The user has to maintain a valid buffer!
    attr_md.vlen = 0;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;

    // CHANGED: 5-Feb-2017
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);

    // This structure holds the actual value of the characteristic. 
    // It also holds the maximum length of the value and it's UUID.
    ble_gatts_attr_t attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = 0;
    attr_char_value.max_len = sizeof(tx_value_buf);
    attr_char_value.init_offs = 0;
    attr_char_value.p_value = tx_value_buf;

    // Attribute Meta for CCCD
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;    

    char_md.p_cccd_md = &cccd_md;


    if ((err_code = sd_ble_gatts_characteristic_add(service_handle, &char_md, &attr_char_value,
                                   p_char_handles)) != NRF_SUCCESS) {
        printf("Error Code - %u\r\n", err_code);
        return -1;
    }
    // m_tx_char_value_handle = p_char_handles->value_handle;
    return 0;
}

int rx_characteristics_add(uint16_t service_handle, ble_gatts_char_handles_t *p_char_handles)
{
    uint32_t err_code;

    ble_uuid_t char_uuid;
    char_uuid.uuid = BLE_UUID_RX_CHARACTERISTC_UUID;
    char_uuid.type = m_ble_uuid_index;

    // It holds the properties of characteristics which will be present in value of characteristics declaration
    // It also has cccd values
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 0;
    char_md.char_props.write = 1;
    // char_md.char_props.notify = 1;
    // char_md.char_props.indicate = 1;  
    char_md.p_cccd_md = NULL;  
    

    // This handles characteristic value declaration attitudes's permissions, loacation to store attritute value and
    // have a flag to set if attribute value is of variable length or not!
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_USER; // The user has to maintain a valid buffer!
    attr_md.vlen = 0;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 1;

    // CHANGED: 5-Feb
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    // This structure holds the actual value of the characteristic. 
    // It also holds the maximum length of the value and it's UUID.
    ble_gatts_attr_t attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = 0;
    attr_char_value.max_len = sizeof(rx_value_buf);
    attr_char_value.init_offs = 0;
    attr_char_value.p_value = rx_value_buf;  


    if ((err_code = sd_ble_gatts_characteristic_add(service_handle, &char_md, &attr_char_value,
                                   p_char_handles)) != NRF_SUCCESS) {
        printf("Error Code - %u\r\n", err_code);
        return -1;
    }
    // m_rx_char_value_handle = p_char_handles->value_handle;
    return 0;
}

int send_indication(uint16_t conn_handle, uint16_t value_handle, uint8_t *p_data, uint16_t len)
{   
    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = value_handle;
    hvx_params.type = BLE_GATT_HVX_INDICATION;
    hvx_params.offset = 0;
    hvx_params.p_len = &len;
    hvx_params.p_data = (uint8_t*)p_data;  

    if (sd_ble_gatts_hvx(conn_handle, &hvx_params) != NRF_SUCCESS)
        return -1;
    return 0;
}

int ble_trs_transmit(uint8_t *p_data, unsigned length)
{
	if (!trs.isHVXEnabled)
		return -1;

	if (length > sizeof(tx_value_buf))
		return -1;

	return send_indication(m_conn_handle, trs.tx_char_handles.value_handle, p_data, length);
}

void gatts_handler(ble_evt_t *p_evt)
{
	ble_gatts_evt_t *p_gatts_evt = &(p_evt->evt.gatts_evt);
	m_conn_handle = (p_gatts_evt->conn_handle);
	

	switch (p_evt->header.evt_id) {
	case BLE_GATTS_EVT_WRITE:
	{
		ble_gatts_evt_write_t *p_write_evt = &(p_gatts_evt->params.write);
		printf("In Gatts Write Event\r\n");

		if (p_write_evt->handle == trs.rx_char_handles.value_handle) {
			if (trs.fp_rx_handler)
				trs.fp_rx_handler(rx_value_buf + p_write_evt->offset, p_write_evt->len);
			break;
		}
        if (p_write_evt->handle == trs.tx_char_handles.value_handle) {
            printf("Invalid Access! Bypassed properties\r\n");
            break;
        }
		if (p_write_evt->handle == trs.tx_char_handles.cccd_handle) {
			if (p_write_evt->len != 2)
				break;
            
			trs.isHVXEnabled = !!(ble_srv_is_indication_enabled(p_write_evt->data));
            if (trs.isHVXEnabled)
                printf("Indication Enabled\r\n");
            else
                printf("Indication Disabled\r\n");
			break;
		}

		break;
	}
	case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
	{
		ble_gatts_evt_rw_authorize_request_t *p_rw_auth_rq = &(p_gatts_evt->params.authorize_request);
		uint8_t type = (p_rw_auth_rq->type);

        printf("In Gatts authorize Event\r\n");

        ble_gatts_rw_authorize_reply_params_t reply;
        memset(&reply, 0, sizeof(reply));
        reply.type = type;
        reply.params.read.gatt_status = BLE_GATT_STATUS_ATTERR_INSUF_AUTHORIZATION;

		if (type == BLE_GATTS_AUTHORIZE_TYPE_INVALID) {
            printf("Error in authorize_request\r\n");
			break;
		}

        if (type == BLE_GATTS_AUTHORIZE_TYPE_READ) {
            sd_ble_gatts_rw_authorize_reply(m_conn_handle, &reply);
            break;
		} 

        if (type == BLE_GATTS_AUTHORIZE_TYPE_WRITE) {   // BLE_GATTS_AUTHORIZE_TYPE_WRITE
            ble_gatts_evt_write_t *p_write_evt = &(p_rw_auth_rq->request.write);
            reply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;

            if (sd_ble_gatts_rw_authorize_reply(m_conn_handle, &reply) != NRF_SUCCESS)
                break;

            if (p_write_evt->handle == trs.rx_char_handles.value_handle) {
                if (trs.fp_rx_handler)
                    trs.fp_rx_handler(rx_value_buf + p_write_evt->offset, p_write_evt->len);
            }
           
		}

        

		break;
	}
	case BLE_GATTS_EVT_SYS_ATTR_MISSING:
	{
	    ble_gatts_evt_sys_attr_missing_t *p_sys_attr = &(p_gatts_evt->params.sys_attr_missing);
	    uint8_t hint = p_sys_attr->hint;
	    printf("Hint - %u\r\n", hint);

        if (sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0) != NRF_SUCCESS) {
            printf("Setting Sys Attr failed\r\n");
            break;
        }
		break;
	}
	case BLE_GATTS_EVT_HVC:
	{
		ble_gatts_evt_hvc_t *p_evt_hvc = &(p_gatts_evt->params.hvc);
		// uint16_t handle = p_evt_hvc->handle;
		if (trs.fp_tx_handler)
				trs.fp_tx_handler(true);
		break;
	}
	case BLE_GATTS_EVT_TIMEOUT:
	{
		ble_gatts_evt_timeout_t *p_evt_to = &(p_gatts_evt->params.timeout);
		uint8_t src = p_evt_to->src;
        printf("Timeout\r\n");

		if (src == BLE_GATT_TIMEOUT_SRC_PROTOCOL) {
			// Do Something!
			if (trs.fp_tx_handler)
				trs.fp_tx_handler(false);
		} else {
			// Do Something else
		}
		break;
	}
	}
}