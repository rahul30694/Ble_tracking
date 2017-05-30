#include "config.h"
#include "utils.h"
#include "serial.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


struct sysConfig_s sysConfig = {
	{
		"Phobos",
		"WWL001",
		"v1.0",

		"WWL001_ap",
		"ab12cd34",

		"arora",
		"netconnect",

		{{ 0, 0, 0, 0 }},
		0,

		{{ 0, 0, 0, 0 }},
		0
	},
	0
};

bool __init__=false;

int configInit(struct sysConfig_s *sysCfg)
{
	memset(sysCfg->cfg.own_ip.byte, 0, sizeof(sysCfg->cfg.own_ip));
	memset(sysCfg->cfg.cloud_ip.byte, 0, sizeof(sysCfg->cfg.cloud_ip));

	memset(&sysCfg->cfg.cloud_port, 0, sizeof(sysCfg->cfg.cloud_port));
	memset(&sysCfg->cfg.own_server_port, 0, sizeof(sysCfg->cfg.own_server_port));

	sysCfg->checksum = util::crc16(&sysCfg->cfg, sizeof(struct sysConfigData_s));

	__init__=true;
	return 0;
}

int sysConfigInit()
{
	return configInit(&sysConfig);
}

void calculateChecksum(struct sysConfig_s *sysCfg)
{
	sysCfg->checksum = util::crc16(&sysCfg->cfg, sizeof(struct sysConfigData_s));

}

int loadConfig(struct sysConfig_s *new_cfg)
{
	//validate checksum
	uint16_t new_checksum = util::crc16(&new_cfg->cfg, sizeof(struct sysConfigData_s));
	if (new_checksum != new_cfg->checksum)
		return -1;

	if (strcmp(sysConfig.cfg.product_id, new_cfg->cfg.product_id))
		return -1;
	if (strcmp(sysConfig.cfg.lock_id, new_cfg->cfg.lock_id))
		return -1;
	if (strcmp(sysConfig.cfg.firmware_version, new_cfg->cfg.firmware_version))
		return -1;

	memcpy(&sysConfig.cfg, (const void *)&new_cfg->cfg, sizeof(struct sysConfigData_s));
	sysConfig.checksum = new_checksum;
	return 0;
}


void printSysCfg()
{
	UARTLogInfo("Product id - %s", sysConfig.cfg.product_id);
	UARTLogInfo("Lock id - %s", sysConfig.cfg.lock_id);
	UARTLogInfo("Firmware version - %s\n", sysConfig.cfg.firmware_version);

	UARTLogInfo("Cloud IP - %d.%d.%d.%d", sysConfig.cfg.cloud_ip.byte[0], sysConfig.cfg.cloud_ip.byte[1], 
		sysConfig.cfg.cloud_ip.byte[2], sysConfig.cfg.cloud_ip.byte[3]);
	UARTLogInfo("Cloud Port - %u", (unsigned)sysConfig.cfg.cloud_port);

	UARTLogInfo("Own IP - %d.%d.%d.%d", sysConfig.cfg.own_ip.byte[0], sysConfig.cfg.own_ip.byte[1], 
		sysConfig.cfg.own_ip.byte[2], sysConfig.cfg.own_ip.byte[3]);
	UARTLogInfo("Own Server Port - %u\n", (unsigned)sysConfig.cfg.own_server_port);

	UARTLogInfo("Ext SSID of AP - %s", sysConfig.cfg.ext_ap_ssid);
	UARTLogInfo("Ext Password of AP - %s", sysConfig.cfg.ext_ap_passw);

	UARTLogInfo("Own SSID of AP - %s", sysConfig.cfg.own_ap_ssid);
	UARTLogInfo("Own Password of AP - %s\n", sysConfig.cfg.own_ap_passw);

	UARTLogInfo("Checksum - 0x%04x\n", sysConfig.checksum);
}

char *getExtSSID()
{
	return sysConfig.cfg.ext_ap_ssid;
}

char *getExtPassw()
{
	return sysConfig.cfg.ext_ap_passw;
}

//#define __MAIN__

#ifdef __MAIN__
int main()
{
	sysConfigInit();
	printSysCfg();
	return 0;
}
#endif