#include "pstorage_for_peer.h"


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#define MAX_BONDS			        10
#define MAX_CONNECTIONS 	        1

#define INVALID_SD_CONN_HANDLE		BLE_CONN_HANDLE_INVALID

struct peer_id_s { 
    ble_gap_id_key_t peer_id;
    uint16_t ediv;

    union {
    	uint8_t id_bitmap;
    	struct {
    		uint8_t assigned:1;
    		uint8_t irk_entry:1;
    		uint8_t addr_entry:1;
    		uint8_t service_context_entry:1;
    	} flags;
    };
};

STATIC_ASSERT(sizeof(struct peer_id_s) % 4 == 0);
 
struct bond_context_s {
    ble_gap_enc_key_t peer_enc_key;
};

STATIC_ASSERT(sizeof(struct bond_context_s) % 4 == 0);


struct conn_instance_s {
    ble_gap_addr_t peer_addr;   
    uint16_t conn_handle;

    union {
    	uint8_t state;
    	struct {
    		uint8_t allocated:1;

    		uint8_t connected:1;
    		uint8_t pairing:1;
    		uint8_t pairing_req_pending:1;
    		uint8_t bonded:1;
    		uint8_t encrypted:1;
    		uint8_t bond_info_update:1;

    		uint8_t disconnecting:1;
    	} flags;
    };

    uint8_t bonded_dev_id;
};


static struct peer_id_s peer_table[MAX_BONDS] ;     
static struct bond_context_s bond_table[MAX_CONNECTIONS];
static struct conn_instance_s  conn_table[MAX_CONNECTIONS]; 
static uint8_t irk_index_table[7];


int conn_instance_allocate(void)
{
	unsigned i;
	for (i=0; i<MAX_CONNECTIONS; i++) {
		if (!conn_table[i].flags.allocated)
			break;
	}
	if (i >= MAX_CONNECTIONS)
		return -1;

	struct conn_instance_s *p = (conn_table + i);
	p->state = 0x00;
	p->conn_handle = INVALID_SD_CONN_HANDLE;
	p->bonded_dev_id = INVALID_BONDED_DEV_ID;
	memset(&(p->peer_addr), 0, sizeof(p->peer_addr));
	return i;
}

void conn_instance_init(unsigned ndx)
{
	struct conn_instance_s *p = (conn_table + ndx);
	p->state = 0x00;
	p->conn_handle = INVALID_SD_CONN_HANDLE;
	p->bonded_dev_id = INVALID_BONDED_DEV_ID;
	memset(&(p->peer_addr), 0, sizeof(p->peer_addr));
}


int find_conn_instance(int conn_handle)
{
	int i;
	for (i=0; i<MAX_CONNECTIONS; i++) {
		if ((conn_table[i].conn_handle == conn_handle) && (conn_table[i].flags.connected))
			return i;
	}
	return -1;
}

void peer_instance_init(unsigned ndx)
{
    memset(&(peer_table[ndx].peer_id.id_addr_info.addr), 0, BLE_GAP_ADDR_LEN);
    peer_table[ndx].peer_id.id_addr_info.addr_type = INVALID_ADDR_TYPE;

    memset(&(peer_table[ndx].peer_id.id_info.irk), 0, BLE_GAP_SEC_KEY_LEN);
    peer_table[ndx].id_bitmap = 0x00;
    peer_table[ndx].ediv = EDIV_INIT_VAL;
}


int peer_instance_allocate(ble_gap_addr_t const *p_addr)
{
    unsigned ndx;
    for (ndx=0; ndx<MAX_BONDS; ndx++) {

        if (!peer_table[ndx].flags.assigned) {
            if (p_addr->addr_type != BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE) {
                peer_table[ndx].flags.addr_entry = 1;
                peer_table[ndx].peer_id.id_addr_info  = (*p_addr);
            } else {
                peer_table[ndx].flags.irk_entry = 1;
            }
            return ndx;
        }
    }
    return -1;
}

int peer_instance_free(unsigned index)
{
    pstorage_handle_t block_handle;

    // Get the block handle.
    // err_code = pstorage_block_identifier_get(&m_storage_handle, device_index, &block_handle);

    if (err_code == NRF_SUCCESS) {
        err_code = pstorage_clear(&block_handle, ALL_CONTEXT_SIZE);

        if (err_code == NRF_SUCCESS) {
            peer_instance_init(device_index);
        }
    }

    return err_code;
}


int peer_instance_find(ble_gap_addr_t const *p_addr, uint16_t ediv)
{
    unsigned ndx;
    for (ndx=0; ndx<MAX_BONDS; ndx++) {

        if (((p_addr == NULL) && (ediv == peer_table[ndx].ediv)) ||
            ((p_addr != NULL) && (memcmp(&(peer_table[ndx].peer_id.id_addr_info), p_addr, sizeof(ble_gap_addr_t)) == 0))) {
            return ndx;
        }
    }
    return -1;
}