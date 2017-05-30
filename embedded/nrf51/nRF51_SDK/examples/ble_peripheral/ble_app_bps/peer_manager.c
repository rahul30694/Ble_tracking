#include "peer_manager.h"
#include "ble_hci.h"
#include "conn_manager.h"
#include "errors.h"

#include <stdio.h>


#define MAX_BONDS			        7
#define MAX_CONNECTIONS 	        1

#define INVALID_SD_CONN_HANDLE		BLE_CONN_HANDLE_INVALID
#define INVALID_BONDED_DEV_ID		0xFF

#define INVALID_ADDR_TYPE			0xFF
#define EDIV_INIT_VAL     			0xFFFF

#define DM_INVALID_ID				0xFF

/**
 * Context offsets each of the context information in persistent memory.
 *
 * Below is a layout showing how each how the context information is stored in persistent
 * memory.
 *
 * All Device context is stored in the flash as follows:
 * +---------+---------+---------+------------------+----------------+--------------------+
 * | Block / Device ID + Layout of stored information in storage block                    |
 * +---------+---------+---------+------------------+----------------+--------------------+
 * | Block 0 | Device 0| Peer Id | Bond Information | Service Context| Application Context|
 * +---------+---------+---------+------------------+----------------+--------------------+
 * | Block 1 | Device 1| Peer Id | Bond Information | Service Context| Application Context|
 * +---------+---------+---------+------------------+----------------+--------------------+
 * |  ...              | ....                                                             |
 * +---------+---------+---------+------------------+----------------+--------------------+
 * | Block N | Device N| Peer Id | Bond Information | Service Context| Application Context|
 * +---------+---------+---------+------------------+----------------+--------------------+
 *
 */

// ble_gap_enc_key_t: It stores encryption key and master id (EDIV and rand)
// Member info of ble_gap_enc_key_t:
//      ble_gap_enc_info_t enc_info
//              uint8_t ltk [16]      // Local IRK
//              uint8_t auth: 1
//              uint8_t ltk_len: 7
//      ble_gap_master_id_t master_id // Central Information (EDIV, IRK, Address)
//              uint16_t ediv          
//              uint8_t rand [8]

// It stores portion of bonding information exchanged by a device during bond creation that needs to be stored persistently.
struct bond_context_s {
    ble_gap_enc_key_t peer_enc_key;
};

STATIC_ASSERT(sizeof(struct bond_context_s) % 4 == 0);



// ble_gap_id_key_t: It has address type, address information and IRK key
// Member info of ble_gap_id_key_t
// 		ble_gap_irk_t 	id_info
//				uint8_t irk [16]
// 		ble_gap_addr_t 	id_addr_info
//				uint8_t addr_type
//				uint8_t addr [6]

// It stores peer identification information

struct bond_context_s;
struct peer_id_s { 
    ble_gap_id_key_t peer_id;
    struct bond_context_s bond_context;
    uint16_t ediv;

    union {
    	uint8_t _bitmap; // Peer identification bits 
    	struct {
    		uint8_t assigned:1;
    		uint8_t irk_entry:1;
    		uint8_t addr_entry:1;
    		uint8_t service_context_entry:1;
    	} flags;
    };
};

STATIC_ASSERT(sizeof(struct peer_id_s) % 4 == 0);



struct conn_instance_s {
    ble_gap_addr_t peer_addr;   
    uint16_t conn_handle;

    union {
    	uint16_t _bitmap;
    	struct {
    		uint8_t allocated:1;

    		uint8_t connected:1;
    		uint8_t pairing:1;
            uint8_t paired:1;
    		uint8_t pairing_req_pending:1; // request made by our device
    		uint8_t bonded:1;
    		uint8_t encrypted:1;
    		uint8_t bond_info_update:1;

    		uint8_t disconnecting:1;
    		uint8_t disconnected:1;

    		uint8_t dirty:1;
    	} flags;
    };

    // Bond Context
    //struct bond_context_s bond_context;

    // Index in peer_id
    uint8_t bonded_dev_id;
};

static struct peer_id_s m_peer_table[MAX_BONDS];     
static struct conn_instance_s m_conn_table[MAX_CONNECTIONS]; 
static uint8_t m_irk_index_table[7];

ble_gap_sec_params_t m_sec_param;

#define isConnBonded(ndx)			(m_conn_table[ndx].flags.bonded && (m_conn_table[ndx].bonded_dev_id != INVALID_BONDED_DEV_ID))

void conn_inst_init(uint32_t ndx)
{
    struct conn_instance_s *p = (m_conn_table + ndx);
    p->_bitmap = 0x00;
    p->conn_handle = INVALID_SD_CONN_HANDLE;
    p->bonded_dev_id = INVALID_BONDED_DEV_ID;
    memset(&(p->peer_addr), 0, sizeof(p->peer_addr));
}

int conn_inst_allocate(uint32_t *p_ndx)
{
	uint32_t i;
	for (i=0; i<MAX_CONNECTIONS; i++) {
		if (!m_conn_table[i].flags.allocated)
			goto DONE;
	}
	// Failed
	return _ENOSPACE;

DONE:
	m_conn_table[i]._bitmap = 0x00;
	m_conn_table[i].flags.allocated = 1;
    *p_ndx = i;
	return 0;
}


void conn_inst_conninfo_fill(uint32_t ndx, uint16_t conn_handle, ble_gap_addr_t *p_addr)
{
	m_conn_table[ndx].conn_handle = conn_handle;
	m_conn_table[ndx].peer_addr = (*p_addr);
	m_conn_table[ndx].flags.connected = 1;
}

void conn_inst_bondinfo_fill(uint32_t ndx, unsigned peer_id_ndx)
{
	m_conn_table[ndx].flags.bonded = 1;
	m_conn_table[ndx].bonded_dev_id = peer_id_ndx;
}


int conn_inst_find(uint32_t *p_ndx, uint16_t conn_handle)
{
	uint32_t i;
	for (i=0; i<MAX_CONNECTIONS; i++) {
		if ((m_conn_table[i].conn_handle == conn_handle) && (m_conn_table[i].flags.connected)) {
            *p_ndx = i;
            return 0;
        }
	}
	return _ENOTFOUND;
}

void conn_inst_free(uint32_t ndx)
{
    conn_inst_init(ndx);
}

void peer_inst_init(uint32_t ndx)
{
    memset(&(m_peer_table[ndx].peer_id.id_addr_info.addr), 0, BLE_GAP_ADDR_LEN);
    m_peer_table[ndx].peer_id.id_addr_info.addr_type = INVALID_ADDR_TYPE;

    memset(&(m_peer_table[ndx].peer_id.id_info.irk), 0, BLE_GAP_SEC_KEY_LEN);
    m_peer_table[ndx]._bitmap = 0x00;
    m_peer_table[ndx].ediv = EDIV_INIT_VAL;
}


int peer_inst_allocate(uint32_t *p_ndx, const ble_gap_addr_t *p_addr)
{
    unsigned i;
    for (i=0; i<MAX_BONDS; i++) {
        if (!m_peer_table[i].flags.assigned) {
            if (p_addr->addr_type != BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE) {
                m_peer_table[i].flags.addr_entry = 1;
                m_peer_table[i].peer_id.id_addr_info = (*p_addr);
            } else {
                m_peer_table[i].flags.irk_entry = 1;
            }
            *p_ndx = i;
            return 0;
        }
    }
    return _ENOSPACE;
}

int peer_inst_free(uint32_t ndx)
{
    peer_inst_init(ndx);
    return 0;
}


int peer_inst_find(uint32_t *p_device_index, ble_gap_addr_t const *p_addr, uint16_t ediv)
{
    unsigned ndx;
    for (ndx=0; ndx<MAX_BONDS; ndx++) {

        if (((p_addr == NULL) && (ediv == m_peer_table[ndx].ediv)) ||
            ((p_addr != NULL) && (memcmp(&(m_peer_table[ndx].peer_id.id_addr_info), p_addr, sizeof(ble_gap_addr_t)) == 0))) {
        	*p_device_index = ndx;
            return 0;
        }
    }
    return _ENOTFOUND;
}

int conn_manager_init()
{
    m_sec_param.bond = SEC_PARAM_BOND;
    m_sec_param.mitm = SEC_PARAM_MITM;
    m_sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
    m_sec_param.oob = SEC_PARAM_OOB;
    m_sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;

	m_sec_param.kdist_central.enc = 0;
	m_sec_param.kdist_central.id = 1;
	m_sec_param.kdist_central.sign = 0;
	m_sec_param.kdist_periph.enc = 1;
	m_sec_param.kdist_periph.id = 1;
	m_sec_param.kdist_periph.sign = 0;

    uint32_t i;
    for (i=0; i<MAX_CONNECTIONS; i++)
        conn_inst_init(i);
    for (i=0; i<MAX_BONDS; i++)
        peer_inst_init(i);
    return 0;
}


void handler(ble_evt_t *p_ble_evt)
{
	ble_gap_evt_t *p_gap = &(p_ble_evt->evt.gap_evt);

	uint16_t conn_handle = p_gap->conn_handle;
	uint32_t peer_index = INVALID_BONDED_DEV_ID;
	uint32_t conn_index = INVALID_SD_CONN_HANDLE;

	if (p_ble_evt->header.evt_id != BLE_GAP_EVT_CONNECTED) {
		if (conn_inst_find(&conn_index, conn_handle))
			return;
		if (isConnBonded(conn_index))
			peer_index = m_conn_table[conn_index].bonded_dev_id;
	}

    switch (p_ble_evt->header.evt_id) {

        case BLE_GAP_EVT_CONNECTED: // Central and Peripheral
        {
        	ble_gap_evt_connected_t *p_evt = &(p_gap->params.connected);
        	if (conn_inst_allocate(&conn_index))
        		return;
            printf("Conn Inst Allocated - %u for conn_handle %d\r\n", conn_index, conn_handle);
        	conn_inst_conninfo_fill(conn_index, conn_handle, &(p_evt->peer_addr));

        	bool bonded_peer_found = false;

        	if (p_evt->irk_match == 1) {
                if (m_irk_index_table[p_evt->irk_match_idx] != DM_INVALID_ID) {
                    peer_index = m_irk_index_table[p_evt->irk_match_idx];
                    bonded_peer_found = true;
                }
            } else {
                if (!peer_inst_find(&peer_index, &(p_evt->peer_addr), EDIV_INIT_VAL))
                	bonded_peer_found = true;
            }

            if (bonded_peer_found) {
               conn_inst_bondinfo_fill(conn_index, peer_index);
            }
      
        	// print_gap_evt_connected(p_evt);
            
            break;
        }

        case BLE_GAP_EVT_DISCONNECTED: // Central and Peripheral
        {
            ble_gap_evt_disconnected_t *p_evt = &(p_gap->params.disconnected);

            m_conn_table[conn_index].flags.connected = 0;
            m_conn_table[conn_index].flags.disconnected = 1;

            if (m_conn_table[conn_index].flags.bonded) {
            	if (m_conn_table[conn_index].flags.encrypted) {
            		// Store or Queue it to store persistently
            	}
            } else {
            	if (m_conn_table[conn_index].bonded_dev_id != INVALID_BONDED_DEV_ID) {
            		peer_inst_init(m_conn_table[conn_index].bonded_dev_id);
            	}

            }
            conn_inst_free(conn_index);
            print_gap_evt_disconnected(p_evt);
            break;
        }
        // The peripheral and the central will get the BLE_GAP_EVT_CONN_PARAM_UPDATE event
        // when central has updated the conn params
        case  BLE_GAP_EVT_CONN_PARAM_UPDATE:
        {
        	// Occur for when connection parameters are updated
			ble_gap_evt_conn_param_update_t *p_evt = &(p_gap->params.conn_param_update);
			print_gap_evt_conn_param_update(p_evt);
        	break;
        }
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        {
            printf("BLE_GAP_EVT_SEC_PARAMS_REQUEST\r\n");
        	ble_gap_evt_sec_params_request_t *evt = &(p_gap->params.sec_params_request);

        	m_conn_table[conn_index].flags.pairing = 1;

        	if (isConnBonded(conn_index)) { // Bond Info Update
        		m_conn_table[conn_index].flags.bond_info_update = 1;
                printf("Already Bonded\r\n");
        	} else {
        		// Assign a peer index as a new bond or update existing bonds
        		if (!peer_inst_find(&peer_index, &m_conn_table[conn_index].peer_addr, EDIV_INIT_VAL)) {
        			conn_inst_bondinfo_fill(conn_index, peer_index);
                    printf("Peer inst found for sec req\r\n");
        		} else {
                    // TODO: Handle Peer allocation fail!
        			if (peer_inst_allocate(&peer_index, &(m_conn_table[conn_index].peer_addr)))
        				break;
                    printf("New peer inst allocated for sec req\r\n");
                    m_conn_table[conn_handle].flags.pairing = 1;
                    m_conn_table[conn_handle].bonded_dev_id = peer_index;
        		}
        	}
            printf("Peer Index at Req - %d\r\n", peer_index);
        	ble_gap_sec_keyset_t keys_xch;

        	keys_xch.keys_central.p_enc_key = NULL;
            keys_xch.keys_central.p_id_key = &(m_peer_table[peer_index].peer_id); 
            keys_xch.keys_central.p_sign_key = NULL;
            keys_xch.keys_periph.p_enc_key = &(m_peer_table[peer_index].bond_context.peer_enc_key);
            keys_xch.keys_periph.p_id_key = NULL;
            keys_xch.keys_periph.p_sign_key = NULL;
            
            uint32_t err_code = sd_ble_gap_sec_params_reply(conn_handle, BLE_GAP_SEC_STATUS_SUCCESS, &m_sec_param, 
                                                   &keys_xch);
            if (err_code != NRF_SUCCESS)
            	break;
            print_gap_evt_sec_params(evt);
            break;
        }
        case BLE_GAP_EVT_SEC_INFO_REQUEST:
        {
            printf("BLE_GAP_EVT_SEC_INFO_REQUEST\r\n");
            ble_gap_evt_sec_info_request_t *p_evt = &(p_gap->params.sec_info_request);

            ble_gap_enc_info_t *p_enc_info = NULL;
			ble_gap_irk_t *p_id_info = NULL;
			ble_gap_sign_info_t *p_sign = NULL; // Not Supported Yet!

        	if (isConnBonded(conn_index)) {
                printf("Connection is already bonded\r\n");
                peer_index = m_conn_table[conn_index].bonded_dev_id;

        		p_enc_info = &(m_peer_table[peer_index].bond_context.peer_enc_key.enc_info);
                p_id_info = &(m_peer_table[peer_index].peer_id.id_info);
        	} else {
                printf("EDIV at info rq - %d\r\n", p_evt->master_id.ediv);
        		if (!peer_inst_find(&peer_index, NULL, p_evt->master_id.ediv))
        			conn_inst_bondinfo_fill(conn_index, peer_index);
                p_enc_info = &(m_peer_table[peer_index].bond_context.peer_enc_key.enc_info);
        	    p_id_info = &(m_peer_table[peer_index].peer_id.id_info);
               
        	}

        	if (sd_ble_gap_sec_info_reply(conn_handle, p_enc_info, p_id_info, NULL) != NRF_SUCCESS) {
        		printf("Sec Info Failed\r\n");
                break;
        	}

            print_gap_evt_sec_info_req(p_evt);
            break;
        }
        // Remove it
        case BLE_GAP_EVT_PASSKEY_DISPLAY:
        {
        	ble_gap_evt_passkey_display_t *p_evt = &(p_gap->params.passkey_display);

        	char ble_pass_key[MAX_BLE_PASSKEY_LEN];
        	snprint_passkey(ble_pass_key, sizeof(ble_pass_key), p_evt);

        	printf("[Passkey Display Evt]=> Passkey: %s\r\n", ble_pass_key);
        	break;
        }

        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
        {
        	ble_gap_evt_auth_key_request_t *evt = &(p_gap->params.auth_key_request);

        	print_gap_evt_auth_key_request(evt);
            char str[19];
            printf("Type Passkey: \r\n");
            if (sd_ble_gap_auth_key_reply(conn_handle, BLE_GAP_AUTH_KEY_TYPE_PASSKEY, str) != NRF_SUCCESS) {
                printf("Passkey Failed\r\n");
            }
        	break;
        }
        case BLE_GAP_EVT_AUTH_STATUS: // Peripheral
        {
            printf("BLE_GAP_EVT_AUTH_STATUS\r\n");
            ble_gap_evt_auth_status_t *p_evt = &(p_gap->params.auth_status);

            m_conn_table[conn_index].flags.pairing = 0;

        	if (p_evt->auth_status != BLE_GAP_SEC_STATUS_SUCCESS) {
                // Free the allocation as bonding failed.
                if (peer_inst_free(m_conn_table[conn_index].bonded_dev_id)) {
                	printf("Memory Leak in DataBase\r\n");
                }
                break;
            }
            if (p_evt->bonded == 0) {
            	// Paring Information!
            	break;
            }

            // Bonding Successfull
            if (m_conn_table[conn_index].bonded_dev_id != INVALID_BONDED_DEV_ID) {
            	m_conn_table[conn_index].flags.bonded = 1;
                peer_index = m_conn_table[conn_handle].bonded_dev_id;
                printf("Peer Index - %d\r\n", peer_index);
            	if (p_evt->kdist_central.id == 1) {
                    m_peer_table[peer_index].flags.irk_entry = 1;
                }

                if (m_conn_table[conn_index].peer_addr.addr_type !=
                                BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE) {
                    m_peer_table[peer_index].peer_id.id_addr_info =
                        m_conn_table[conn_index].peer_addr;
                    m_peer_table[peer_index].flags.addr_entry = 1;

                              
                } else {
                    // Here we must fetch the keys from the keyset distributed.
                    m_peer_table[peer_index].ediv = m_peer_table[peer_index].bond_context.peer_enc_key.master_id.ediv;
                    printf("EDIV at auth - %d\r\n", m_peer_table[peer_index].ediv);
                    m_peer_table[peer_index].flags.irk_entry = 1;
                
                    // device_context_store(&handle, FIRST_BOND_STORE);
                } 
            }
            print_gap_evt_auth_status(p_evt);
        	break;
        }

        case BLE_GAP_EVT_CONN_SEC_UPDATE: // Connection Security Updated!
        {
            printf("BLE_GAP_EVT_CONN_SEC_UPDATE\r\n");
            ble_gap_evt_conn_sec_update_t *p_evt = &(p_gap->params.conn_sec_update);

            if ((p_evt->conn_sec.sec_mode.sm == 1) && (p_evt->conn_sec.sec_mode.lv == 1) && (m_conn_table[conn_index].flags.bonded)) {
            	// Lost bond case, generate a security refresh event!
            	m_conn_table[conn_index].flags.pairing_req_pending = 1;
            	m_conn_table[conn_index].flags.bond_info_update = 1;
            } else {
            	m_conn_table[conn_index].flags.encrypted = 1;
            }
            print_gap_evt_sec_update(p_evt);
            
            break;
        }

        case BLE_GAP_EVT_TIMEOUT: // Don't Know
        {
        	ble_gap_evt_timeout_t *p_evt = &(p_gap->params.timeout);
        	print_gap_evt_timeout(p_evt);
        	break;
        }


        case BLE_GAP_EVT_RSSI_CHANGED: // Don't Know
        {
        	ble_gap_evt_rssi_changed_t *p_evt = &(p_gap->params.rssi_changed);
        	int8_t rssi = p_evt->rssi;
        	printf("[RSSI Changed evt]=> RSSI: %d\r\n", p_evt->rssi);
        	break;
        }

        case BLE_GAP_EVT_ADV_REPORT: // Central Only
        {
        	/* Report after Performing Scanning Received Data could be Adv Pkt or Scan Response	*/
        	ble_gap_evt_adv_report_t *p_evt = &(p_gap->params.adv_report);
        	print_adv_report(p_evt);
        	break;
        }

        case BLE_GAP_EVT_SEC_REQUEST: // Central Only
        {    
        	ble_gap_evt_sec_request_t *p_evt = &(p_gap->params.sec_request);
        	
        	printf("[Security Request Evt]=> Type: %s %s\r\n", (p_evt->bond == 1)?"bond":"", (p_evt->mitm == 1)?"mitm":"");
            break;
        }
        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: // Central only
        {
        	/* This should be used when it is Central */
        	ble_gap_evt_conn_param_update_request_t *p_update_req = &(p_gap->params.conn_param_update_request);
        	ble_gap_conn_params_t *p_conn_params = &(p_update_req->conn_params);

        	char buf[100];
        	snprint_conn_params(buf, sizeof(buf), p_conn_params);
        	printf("[Conn Param Update]=> Param: %s\r\n", buf);
        	break;
        }
        case BLE_GAP_EVT_SCAN_REQ_REPORT: // Peripheral and Central
        {
        	/* If Advertising being a peripheral, the this event is trigerrred when it receives a scan request */
        	ble_gap_evt_scan_req_report_t *p_evt = &(p_gap->params.scan_req_report);
        	ble_gap_addr_t *p_peer_addr = &(p_evt->peer_addr);

        	char ble_addr_buf[MAX_BLE_ADDR_CSTR_LEN];
        	snprint_ble_addr(ble_addr_buf, MAX_BLE_ADDR_CSTR_LEN, p_peer_addr);

        	printf("[Scan Req Report]=> RSSI: %d, Peer Addr: %s\r\n", p_evt->rssi, ble_addr_buf);
        	break;
        }

        default:
        {
        	printf("[Non Gap Evt]\r\n");
            break;
        }
    }

}