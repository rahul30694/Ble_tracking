#include "cstring.h"

#include <string.h>

size_t strlcpy(char *dest, const char *src, size_t n)
{
	if (!dest)
		return 0;

	unsigned i;
	for (i=0; i<(n-1) && src[i]; i++)
		dest[i] = src[i];
	dest[i]='\0';

	return i;
}

unsigned strnumchr(const char *cstr, char c)
{
	unsigned i;
	for (i=0; *cstr; cstr++)
		if (*cstr == c) i++;
	return i;
}

int strposchr(char *cstr, char c)
{
	char *cptr;
	if (!(cptr = strchr(cstr, c)))
		return -1;
	return (cstr - cptr);
}
