/*
 * serial.h
 *
 *  Created on: 22-Jan-2016
 *      Author: abhiarora
 */

#ifndef HAL_SERIAL_H_
#define HAL_SERIAL_H_
 
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#define DISPLAY_FILENAME
#define DISPLAY_LINE_NUMBER

enum logLevel_e {
	LOG_EMERG=0,	/* system is unusable */
	LOG_ALERT,	 	/* action must be taken immediately */
	LOG_CRIT,		/* critical conditions */
	LOG_ERR,		/* error conditions */
	LOG_WARNING,	/* warning conditions */
	LOG_NOTICE,		/* normal but significant condition */
	LOG_INFO,		/* informational */
	LOG_DEBUG		/* debug-level messages */
};

int UARTLogLine(enum logLevel_e level, const char *fileName, int lineNumber, bool eol, const char *format, ...);

#define __FILENAME__ ((strrchr(__FILE__, '/')) ? strrchr(__FILE__, '/') + 1: __FILE__)

#define serialPrintln(...) UARTLogLine(LOG_DEBUG, __FILENAME__, __LINE__, true, __VA_ARGS__)
#define serialPrint(...) UARTLogLine(LOG_DEBUG, __FILENAME__, __LINE__, false, __VA_ARGS__)

#define UARTLogInfo(...) UARTLogLine(LOG_INFO, NULL, -1, true, __VA_ARGS__)

#endif /* HAL_SERIAL_H_ */
