#include "Arduino.h"
#include "esp12.h"
#include "circularBuffer.h"
#include "time.h"
#include "clock.h"
#include "serial.h"
#include "json.h"
#include "cstring.h"
#include "utils.h"
#include "config.h"

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Sleep Mode */
#define DISABLE_SLEEP_MODE  0
#define LIGHT_SLEEP_MODE    1
#define MODEM_SLEEP_MODE    2

/* WiFi Mode */
#define STATION_MODE        1
#define SOFT_AP_MODE        2
#define AP_STATION_MODE     3

/* Connections */
#define SINGLE_CONN         0
#define MULTI_CONN          1


//Formated Command Strings
const char ATCMD_AYT[]="AT\r\n";
const char ATCMD_RST[]="AT+RST\r\n";
const char ATCMD_NOECHO[]="ATE0\r\n";
const char ATCMD_FACTORT_RST[]="AT+RESTORE\r\n";

const char ATCMD_WIFI_MODE[]="AT+CWMODE=%d\r\n";
const char ATCMD_JOIN_AP[]="AT+CWJAP=\"%s\",\"%s\"\r\n";
const char ATCMD_LIST_AP[]="AT+CWLAP=\"%s\"\r\n";
const char ATCMD_DISCONNECT_AP[]="AT+CWQAP\r\n";

const char ATCMD_CONN_STATUS[]="AT+CIPSTATUS\r\n";
const char ATCMD_START_CONN_TCP[]="AT+CIPSTART=%d,\"%s\",\"%s\",\"%s\",%d\r\n";
const char ATCMD_SEND_DATA[]="AT+CIPSEND=%d,%d\r\n";
const char ATCMD_TCP_CLOSE[]="AT+CIPCLOSE=%d\r\n";
const char ATCMD_GET_LOCAL_IP[]="AT+CIFSR\r\n";

const char ATCMD_EN_MULTIPLE_CONN[]="AT+CIPMUX=%d\r\n";
const char ATCMD_TCP_SERVER[]="AT+CIPSERVER=%d,%d\r\n";
const char ATCMD_TCP_TIMEOUT_SEC[]="AT+CIPSTO=%d\r\n";
const char ATCMD_PING[]="AT+PING=\"%s\"\r\n";


//Response Strings
const char ATRESP_OK[]="OK";
const char ATRESP_ERR[]="ERROR";
const char ATRESP_FAIL[]="FAIL";

const char ATRESP_NO_CHANGE[]="no change";
const char ATRESP_PROMPT[]=">";

const char ATRESP_READY[]="ready";
const char ATRESP_SEND_OK[]="SEND OK";


/* Only Two Connection are supported */
struct esp_status_s {
	union {
		uint8_t flags;
		struct {
			uint8_t isActive:1;
			uint8_t isConfigured:1;

			uint8_t isWifiConnected:1;
			uint8_t hasValidIP:1;

			uint8_t isServerUp:1;

			uint8_t isConnToClient:1; //being Server
			uint8_t isConnToServer:1; //being Client

			uint8_t :1;
		};
	};
	int8_t clientLinkID;
	int8_t serverLinkID;

	struct ipv4_s ip;
	char state;
};

ATEngine_c esp_engine;

struct esp_status_s esp_status;

CIRCBUF_DEF(uint8_t, outBuf, 100);

int esp_attention(uint8_t cmdNdx, char *resp)
{
 	if (!strstr(resp, ATRESP_OK))
 		return -1;
	serialPrintln("esp_status12 Connected");
	esp_status.isActive = true;
	return 0;
}


std::string cmd;
bool device1, device2, device3, device4;
void false_response()
{  
	cmd = "HTTP/1.1 200 OK\r\n";
	cmd += "Content-Type: text/html\r\n";
	cmd += "Connection: close\r\n";
	cmd += "Refresh: 15\r\n";
	cmd += "\r\n";
	cmd += "<!DOCTYPE HTML>\r\n";
	cmd += "<html>\r\n";
	cmd += "<header><title>ESP8266 Webserver</title><h1>\"ESP8266 Web Server Control\"</h1></header>";


	if (device1)
		cmd += ("<br/>Device1  : ON");
	else
		cmd += ("<br/>Device1  : OFF");

	if (device2)
		cmd += ("<br/>Device2  : ON");
	else
		cmd += ("<br/>Device2  : OFF");


	if (device3)
		cmd += ("<br/>Device3  : ON");
	else
		cmd += ("<br/>Device3  : OFF");
	
	if (device4)
		cmd += ("<br/>Device4  : ON");
	else
		cmd += ("<br/>Device4  : OFF");

	cmd += "<html>\r\n";
}

void http_parse(char *buf)
{
	buf = strstr(buf, "GET");
	if (!strncmp(buf, "GET /?status1=1", 15)) {
		serialPrintln("Turn on");
		device1 = true;
	} else if (!strncmp(buf, "GET /?status1=0", 15)) {
		serialPrintln("Turn off");
		device1 = false;
	}

	if (!strncmp(buf, "GET /?status2=1", 15)) {
		serialPrintln("Turn on 2 ");
		device2 = true;
	}
	else if (!strncmp(buf, "GET /?status2=0", 15)) {
		serialPrintln("Turn off 2 ");
		device2 = false;
	}
	
}

int handle_client(int linkID, char *resp)
{
	return -1;
}

int handle_server(int linkID, char *resp)
{
	int len, id;
	sscanf(resp, "+IPD,%d,%d:", &id, &len);

	if (linkID != id)
		return -1;

	if (esp_status.isConnToClient)
		return -1;
	esp_status.isConnToClient = true;
	esp_status.clientLinkID = linkID;

	http_parse(resp);
	false_response();
	esp_engine.queue_cmd(SEND_DATA, linkID, cmd.length());
	esp_engine.queue_data((char *)cmd.c_str(), cmd.length());
	esp_engine.queue_cmd(TCP_CLOSE, linkID);

	esp_status.isConnToClient = false;
	esp_status.clientLinkID = -1;

	return 0;
}

int esp_unsolicited_handler(char *resp)
{
	int linkID;
	if (strstr(resp, "CONNECT")) {
		sscanf(resp, "%d,CONNECT", &linkID);
		if (esp_status.isConnToServer && (esp_status.clientLinkID == linkID))
			return handle_client(linkID, resp);

		if (esp_status.isServerUp)
			return handle_server(linkID, resp);
		return -1;
	}

	if (strstr(resp, "CLOSED")) {
		sscanf(resp, "%d,CLOSED", &linkID);
		if (esp_status.isConnToServer && (esp_status.serverLinkID == linkID)) {
			esp_status.isConnToServer = false;
			esp_status.serverLinkID = -1;
			return 0;
		}
		if (esp_status.isServerUp) {
			if (esp_status.isConnToClient) {
				esp_status.isConnToClient = false;
				esp_status.clientLinkID = -1;
				return 0;
			}
		}
		return -1;
	}

	if (strstr(resp, ATRESP_READY)) {
		//reset
		esp_status.flags = 0x00;
		return 0;
	}
	return -1;
}

int esp_get_ip(unsigned char cmdNdx, char *resp)
{
	if (!strstr(resp, ATRESP_OK))
		return -1;
	char ip_cstr[20];
	sscanf(resp, "+CIFSR:STAIP,\"%s\"", ip_cstr);
	util::cstrToIP(&esp_status.ip, ip_cstr);
	util::printIP(&esp_status.ip); serialPrintln("");
	esp_status.hasValidIP = true;
	return 0;

}

int esp12_data_handler(char *resp)
{
	if (!strstr(resp, ATRESP_SEND_OK))
		return -1;
	serialPrintln("Data Send Correct");
	return 0;
}

int esp_join_ap(unsigned char cmdNdx, char *resp)
{
	if (strstr(resp, ATRESP_OK)) {
		esp_status.isWifiConnected = true;
		serialPrintln("Wifi Connected");
		return 0;
	}	
	if (strstr(resp, "WIFI DISCONNECTED") && strstr(resp, ATRESP_FAIL))
		esp_status.isWifiConnected = false;
	
	return -1;
}

int esp_server_callback(unsigned char cmdNdx, char *resp)
{
	if (!strstr(resp, ATRESP_OK))
		return -1;
	esp_engine.unsolicitedAccept(true);
	esp_status.isServerUp = true;
	serialPrintln("Server up and running");
	return 0;
}


struct ATCommand_s espATCommandList[AT_SENTIMEL] = {
	[AYT]              =    {ATCMD_AYT,                ATRESP_OK,    esp_attention,   1000},
	[RESET]            =    {ATCMD_RST,                ATRESP_READY, NULL,            2000}, 
	[NO_ECHO]          =    {ATCMD_NOECHO,             ATRESP_OK,    NULL,            1000},
	[FACTORY_RESTORE]  =    {ATCMD_FACTORT_RST,        ATRESP_READY, NULL,            1000},

	[WIFI_MODE]        =    {ATCMD_WIFI_MODE,          ATRESP_OK,    NULL,			  1000},
	[JOIN_AP]          =    {ATCMD_JOIN_AP,            ATRESP_OK,    esp_join_ap,    15000},
	[LIST_A_AP]        =    {ATCMD_LIST_AP,            ATRESP_OK,    NULL,            1000},
	[DISCONNECT_AP]    =    {ATCMD_DISCONNECT_AP,      ATRESP_OK,    NULL,            1000},

	[CONN_STATUS]      =    {ATCMD_CONN_STATUS,        ATRESP_OK,    NULL,            1000},
	[START_CONN_TCP]   =    {ATCMD_START_CONN_TCP,     ATRESP_OK,    NULL,            1000},
	[SEND_DATA]        =    {ATCMD_SEND_DATA,          ATRESP_PROMPT, NULL,           1000},
	[TCP_CLOSE] 	   =    {ATCMD_TCP_CLOSE, 			ATRESP_OK,    NULL, 		   1000},
	[GET_LOCAL_IP]     =    {ATCMD_GET_LOCAL_IP,       ATRESP_OK, 	  esp_get_ip,      1000},

	[EN_MULTIPLE_CONN] =    {ATCMD_EN_MULTIPLE_CONN,   ATRESP_OK,    NULL,            1000},
	[START_TCP_SERVER] =    {ATCMD_TCP_SERVER,         ATRESP_OK, esp_server_callback, 1000},

	[TCP_TIMEOUT_SEC]  =    {ATCMD_TCP_TIMEOUT_SEC,    ATRESP_OK,    NULL,            1000},
	[PING]             =    {ATCMD_PING,               ATRESP_OK,    NULL,            1000},
};


void esp12_init(char *p)
{
	pinMode(ESP12_EN, OUTPUT);
	pinMode(ESP12_RESET, OUTPUT);

	digitalWrite(ESP12_RESET, LOW);
	esp12_enable();

	esp_engine.init(p, &outBuf, espATCommandList, AT_SENTIMEL);

	esp_status.flags = 0x00;
	esp_status.state = 'i';
	esp_status.clientLinkID = -1;
	esp_status.serverLinkID = -1;

	esp_engine.bindUnsolicitedHandler(esp_unsolicited_handler);
	esp_engine.unsolicitedAccept(false);
	esp_engine.bindDataHandler(esp12_data_handler);

	//esp12_basic_configure();
	esp12_sft_reset();
	
}

int esp12_update()
{
	return esp_engine.updateEngine(millis());
}

int esp_boot_settings();
void esp12_basic_configure()
{
	esp_boot_settings();
}
	

void esp12_hard_reset()
{
	digitalWrite(ESP12_EN, LOW);
	delay(100);
	digitalWrite(ESP12_EN, HIGH);
	delay(100);
	esp_engine.reset_engine();
	esp_status.flags = 0x00;
}

void esp12_sft_reset()
{
	esp_engine.queue_cmd(RESET);
}

void esp12_enable()
{
	digitalWrite(ESP12_EN, HIGH);
}

void esp12_disable()
{
	digitalWrite(ESP12_EN, LOW);
}


#define DEFAULT_WAIT   3000
#define MAX_TRIES         5

int esp_boot_settings()
{
	static char next_state;
	static char prev_state;
	static int tries = MAX_TRIES;

	switch(esp_status.state) {
	case 'i':  // check for ESP for being responsive
	{
		esp_engine.queue_cmd(AYT);

		prev_state = esp_status.state;
		esp_status.state = 'w';
		next_state = 'c';
		break;
	}
	case 'c':
	{
		esp_engine.queue_cmd(NO_ECHO);
		esp_engine.queue_cmd(WIFI_MODE, STATION_MODE);
		esp_engine.queue_cmd(EN_MULTIPLE_CONN, MULTI_CONN);

		prev_state = esp_status.state;
		esp_status.state = 'w';
		next_state = 'p';
		break;
	}
	case 'p':
	{
		esp_engine.queue_cmd(JOIN_AP, getExtSSID(), getExtPassw());
		esp_engine.queue_cmd(GET_LOCAL_IP);
		esp_engine.queue_cmd(START_TCP_SERVER, 1, 80);

		prev_state = esp_status.state;
		esp_status.state = 'w';
		next_state = 's';
		break;
	}
	case 's':
	{
		serialPrintln("Server On");
		//esp_engine.queue_cmd(START_TCP_SERVER, 1, 80);

		prev_state = esp_status.state;
		esp_status.state = 'w';
		next_state = 'o';
		break;
	}
	case 'o':
	{
		//if (esp12_run())
			//esp_status.state = 'r';
		break;
	}
	case 'w':
	{
		if (esp_engine.isStable()) {
			esp_status.state = next_state;
			tries = MAX_TRIES;
			break;
		}

		if (!esp_engine.hasErr())
			break;
	
		esp_engine.reset_engine();
		esp_engine.clearErr();

		if (tries--)
			esp_status.state = prev_state;
		else
			esp_status.state = 'r';
		break;
	}
	case 'r':
	{
		esp_engine.reset_engine();
		esp_engine.clearErr	();
		esp12_hard_reset();
		tries = MAX_TRIES;
		esp_status.state = 'i';
		break;
	}
	}
	return 0;
	

}

