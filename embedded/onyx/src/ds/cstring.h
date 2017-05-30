/*
 * strings.h
 *
 *  Created on: 02-Feb-2016
 *      Author: abhiarora
 */

#ifndef _CSTRING_H_
#define _CSTRING_H_

#include <stddef.h>
#include <string.h>

/*
 * strlcpy() function is similar to strncpy(), but it copies at most size-1 bytes to dest,
 * always adds a terminating null byte, and does not pad the target with (further) null bytes.
 * This function fixes some of the problems of strcpy() and strncpy(),
 * but the caller must still handle the possibility of data loss if size is too small.
 * The return value of the function is the length of src, which allows truncation to be easily detected.
 *
*/

size_t strlcpy(char *dest, const char *src, size_t n);

unsigned strnumchr(const char *cstr, char c);

int strposchr(char *cstr, char c);

#endif /* HELPER_STRINGS_H_ */
