#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <stdint.h>

#define LOCKED_STATE         "l"
#define UNLOCKED_STATE       "u"

#define DOOR_CLOSED          "d"
#define DOOR_OPEN		     "o"

#define WIFI_CONNECTED       "c"
#define WIFI_NOT_CONNECTED   "n"


enum {
	SYS_PARAM_LOCKED_NUM,
	SYS_PARAM_ERR_CODE,
	SYS_PARAM_WIFI_STATUS,
	SYS_PARAM_LOCK_STATE,
	SYS_PARAM_DOOR_STATE,
	SYS_PARAM_BATTERY,

	SYS_PARAM_SENTIMEL
};

int sysParamInit();

int getSysVal(char *name, char *value);
int getSysVal(unsigned ndx, char *value);

int updateSysVal(char *name, const char *value);
int updateSysVal(unsigned ndx, const char *value);
int updateSysVal(char *name, int value);
int updateSysVal(unsigned ndx, int value);


#endif