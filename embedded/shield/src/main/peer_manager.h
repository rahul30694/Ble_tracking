#ifndef _PEER_MANAGER_H_
#define _PEER_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>
#include "sdk_common.h"
#include "ble.h"
#include "ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

void handler(ble_evt_t *p_ble_evt);
int conn_manager_init();

#ifdef __cplusplus
}
#endif

#endif
