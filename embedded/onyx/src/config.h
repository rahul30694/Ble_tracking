#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "type.h"

#include <stdint.h>

#define FIRMWARE_VERSION	"v1.0"


#define MAX_CLOUD_DOMAIN_LEN  30
#define MAX_PRODUCT_ID_LEN    10
#define MAX_LOCK_ID_LEN       10
#define MAX_USER_ID_LEN		  10
#define MAX_FIRMWARE_VER_LEN  10

#define OWN_AP_SSID_MAX_LEN	  10
#define OWN_AP_PASSW_MAX_LEN  10

#define EXT_AP_SSID_MAX_LEN   20
#define EXT_AP_PASSW_MAX_LEN  20


struct sysConfigData_s {
	char product_id[MAX_PRODUCT_ID_LEN];
	char device_id[MAX_LOCK_ID_LEN];
	char user_id[MAX_USER_ID_LEN];

	char firmware_version[MAX_FIRMWARE_VER_LEN];

	char own_ap_ssid[EXT_AP_SSID_MAX_LEN];
	char own_ap_passw[EXT_AP_PASSW_MAX_LEN];

	char ext_ap_ssid[EXT_AP_SSID_MAX_LEN];
	char ext_ap_passw[EXT_AP_PASSW_MAX_LEN];

	struct ipv4_s own_ip; //static ip
	uint16_t own_server_port;

	char cloud[MAX_CLOUD_DOMAIN_LEN];
	uint16_t cloud_port;
	
}__attribute__((packed));

struct sysConfig_s {
	struct sysConfigData_s cfg;
	uint16_t checksum;
}__attribute__((packed));


int sysConfigInit(void);
int loadConfig(struct sysConfig_s *new_cfg);
void calculateChecksum(struct sysConfig_s *sysCfg);

void printSysCfg();

char *getExtSSID();
char *getExtPassw();

#endif