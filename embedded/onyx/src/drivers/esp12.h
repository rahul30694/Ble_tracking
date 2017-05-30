#ifndef _ESP_12_H_
#define _ESP_12_H_

#include "ATEngine.h"

#define ESP12_EN 	2
#define ESP12_TX 	0
#define ESP12_RX 	1
#define ESP12_RESET 3

//Commands Enumeration
enum ATCommand_e {
	AYT,
	RESET,
	NO_ECHO,
	FACTORY_RESTORE,

	WIFI_MODE,
	JOIN_AP,
	LIST_A_AP,
	DISCONNECT_AP,
	
	CONN_STATUS,
	START_CONN_TCP,
	SEND_DATA,
	TCP_CLOSE,
	GET_LOCAL_IP,

	EN_MULTIPLE_CONN,
	START_TCP_SERVER,

	TCP_TIMEOUT_SEC,
	PING,

	AT_SENTIMEL
};


void esp12_init(char *p);
int esp12_update();
void esp12_basic_configure();

void esp12_hard_reset();
void esp12_sft_reset();

void esp12_enable();
void esp12_disable();


#endif