#include "serial.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

enum logLevel_e sysLogLevel = LOG_DEBUG;

int UARTLogLine(enum logLevel_e level, const char *fileName, int lineNumber, bool eol, const char *format,...)
{
	if (level > sysLogLevel)
		return -1;

	int numCharWrite = 0;
	static bool lastEOLState = true;
	
	va_list args;
	va_start(args, format) ;

	#ifdef DISPLAY_FILENAME
	if (lastEOLState && fileName)
		numCharWrite += printf("%s: ", fileName);
	#endif

	#ifdef DISPLAY_LINE_NUMBER
	if (lastEOLState && (lineNumber >= 0))
		numCharWrite += printf("%d: ", lineNumber);
	#endif


	lastEOLState = eol;
	numCharWrite += vprintf(format, args);

	if (eol)
		numCharWrite += printf("\n");
	va_end(args) ;
	return numCharWrite;
}
