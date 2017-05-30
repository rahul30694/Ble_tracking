#ifndef _JSON_FORMER_H_
#define _JSON_FORMER_H_

#include <stdbool.h>
#include <stdarg.h>

#define MAX_JS_STR_LEN 128

int jsonStart();
int jsonPrintf(const char *key, const char *format, ...);
int jsonEnd();

bool isJSFormed();

int jsonFormer_init(char *jsonString, int len);


#endif
