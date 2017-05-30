#include "system.h"
#include "cstring.h"
#include "json_former.h"
#include "serial.h"
#include "time.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#define MAX_VAL_SIZE 4U

rtcDateTime_t sysTime = RTCDATE2001JAN01;
bool sys_sync = false;

struct sysParams_s {
	const char *const name;
	char value[MAX_VAL_SIZE];
};

struct sysParams_s sys_params[SYS_PARAM_SENTIMEL] = {
	[SYS_PARAM_LOCKED_NUM]      =    {"num"},
	[SYS_PARAM_ERR_CODE]        =    {"err"},
	[SYS_PARAM_WIFI_STATUS]     =    {"wifi"},
	[SYS_PARAM_LOCK_STATE]      =    {"lock"},
	[SYS_PARAM_DOOR_STATE]      =    {"door"},
	[SYS_PARAM_BATTERY]         =    {"batt"}
};

int sysParamInit()
{
	strlcpy(sys_params[SYS_PARAM_LOCKED_NUM].value, "0", MAX_VAL_SIZE);
	strlcpy(sys_params[SYS_PARAM_ERR_CODE].value, "0", MAX_VAL_SIZE);
	strlcpy(sys_params[SYS_PARAM_WIFI_STATUS].value, WIFI_NOT_CONNECTED, MAX_VAL_SIZE);
	strlcpy(sys_params[SYS_PARAM_LOCK_STATE].value, UNLOCKED_STATE, MAX_VAL_SIZE);
	strlcpy(sys_params[SYS_PARAM_DOOR_STATE].value, DOOR_CLOSED, MAX_VAL_SIZE);
	strlcpy(sys_params[SYS_PARAM_BATTERY].value, "0", MAX_VAL_SIZE);

	return 0;
}

int getSysVal(char *name, char *value)
{
	unsigned i;
	for (i=0; i<SYS_PARAM_SENTIMEL; i++)
		if (!strcmp(sys_params[i].name, name))
			break;
	return getSysVal(i, value);
}

int getSysVal(unsigned ndx, char *value)
{
	if (ndx >= SYS_PARAM_SENTIMEL)
		return -1;

	int len = strnlen(sys_params[ndx].value, MAX_VAL_SIZE);
	memcpy(value, sys_params[ndx].value, len);
	value[len] = '\0';
	return 0;
}

int updateSysVal(char *name, const char *value)
{
	unsigned i;
	for (i=0; i<SYS_PARAM_SENTIMEL; i++)
		if (!strcmp(sys_params[i].name, name))
			break;
	return updateSysVal(i, value);
}

int updateSysVal(unsigned ndx, const char *value)
{
	if (ndx >= SYS_PARAM_SENTIMEL)
		return -1;

	unsigned len = strnlen(value, MAX_VAL_SIZE);
	if (len >= MAX_VAL_SIZE)
		len -= 1;
	memcpy(sys_params[ndx].value, value, len);
	sys_params[ndx].value[len] = '\0';
	return 0;	
}


int updateSysVal(char *name, int value)
{
	char str_value[MAX_VAL_SIZE];
	sprintf(str_value, "%d", value);
	return updateSysVal(name, (const char *)str_value);
}

int updateSysVal(unsigned ndx, int value)
{
	char str_value[MAX_VAL_SIZE];
	sprintf(str_value, "%d", value);
	return updateSysVal(ndx, (const char *)str_value);
}


int changeLockStateTo(const char *new_state)
{
	return updateSysVal(SYS_PARAM_LOCK_STATE, new_state);
}

int changeWifiStateTo(const char *new_state)
{
	return updateSysVal(SYS_PARAM_WIFI_STATUS, new_state);
}

int changeSysErrTo(int err)
{

	return updateSysVal(SYS_PARAM_ERR_CODE, err);
}

int changeDoorStateTo(const char *new_state)
{
	return updateSysVal(SYS_PARAM_DOOR_STATE, new_state);
}

int changeBattValueTo(int batt_level)
{
	if (batt_level > 100)
		return -1;
	if (batt_level < 0)
		return -1;
	return updateSysVal(SYS_PARAM_BATTERY, batt_level);
}

int changeLockNumTo(int num)
{
	return updateSysVal(SYS_PARAM_LOCKED_NUM, num);
}

bool getSysParamJS(char *js, size_t max)
{
	if (jsonFormer_init(js, max))
		return false;

	jsonStart();
	int i;
	for (i=0; i<SYS_PARAM_SENTIMEL; i++)
		jsonPrintf(sys_params[i].name, sys_params[i].value);
	jsonEnd();
	return isJSFormed();
}

//#define __MAIN__

#ifdef __MAIN__
int main()
{
	sysParamInit();
	changeLockStateTo(LOCKED_STATE);
	changeBattValueTo(100);
	changeLockNumTo(10);
	changeSysErrTo(20);
	changeDoorStateTo(DOOR_OPEN);
	changeWifiStateTo(WIFI_CONNECTED);
	char js[100];
	getSysParamJS(js, 100);
	serialPrintln("sys param - %s", js);
	return 0;
}
#endif