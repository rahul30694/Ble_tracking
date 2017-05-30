#include "json_former.h"
#include "serial.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

enum jsFormerState_e {
	JSON_START, JSON_END, JSON_MID
};


struct jsonFormer_s {
	char *jsStr;
	int jsOffset;
	int strLen;
	enum jsFormerState_e state;

	bool formed;
};

struct jsonFormer_s jsForm;

int jsonStart()
{
	if (jsForm.state != JSON_END)
		return -1;

	jsForm.jsOffset += snprintf(jsForm.jsStr + jsForm.jsOffset, jsForm.strLen - jsForm.jsOffset, "{");
	jsForm.state = JSON_START;
	return 0;
}

int jsonValueQuote=1;
void jsonValueQuoting(int b)
{
	jsonValueQuote=b;
}

int jsonPrintf(const char *key, const char *format, ...)
{
	if (jsForm.state == JSON_END)
		return -1;

	if (jsForm.state == JSON_MID)
		jsForm.jsOffset += snprintf(jsForm.jsStr + jsForm.jsOffset, jsForm.strLen - jsForm.jsOffset, ",");

	va_list(args);
	va_start(args, format);
	jsForm.jsOffset += snprintf(jsForm.jsStr + jsForm.jsOffset, jsForm.strLen - jsForm.jsOffset, "\"%s\":", key);
	if (jsonValueQuote)
		jsForm.jsOffset += snprintf(jsForm.jsStr + jsForm.jsOffset, jsForm.strLen - jsForm.jsOffset, "\"");
	jsForm.jsOffset += vsnprintf(jsForm.jsStr + jsForm.jsOffset, jsForm.strLen - jsForm.jsOffset, format, args);
	if (jsonValueQuote)
		jsForm.jsOffset += snprintf(jsForm.jsStr + jsForm.jsOffset, jsForm.strLen - jsForm.jsOffset, "\"");

	va_end(args);

	if (jsForm.state == JSON_START)
		jsForm.state = JSON_MID;

	return 0;
}

int jsonEnd()
{
	if (jsForm.state != JSON_MID)
			return -1;

	strncat(jsForm.jsStr, "}", jsForm.strLen);
	jsForm.state = JSON_END;
	jsForm.formed = true;
	return 0;
}

bool isJSFormed()
{
	return jsForm.formed;
}

int jsonFormer_init(char *jsonString, int len)
{
	if (!jsonString)
		return -1;

	if (len >= MAX_JS_STR_LEN)
		return -1;

	jsForm.jsStr = jsonString;
	jsForm.jsOffset = 0;
	jsForm.state = JSON_END;
	jsForm.formed = false;
	jsForm.strLen = len;
	return 0;
}

//#define __MAIN__

#ifdef __MAIN__
int main()
{
	char js[100];

	jsonFormer_init(js, 100);
	jsonStart();
	jsonPrintf("Hello World", "tata");
	jsonPrintf("Hel", "ta");
	jsonPrintf("rld", "ta");
	jsonEnd();
	serialPrintln("Json is %s", js);
	return 0;
}
#endif

