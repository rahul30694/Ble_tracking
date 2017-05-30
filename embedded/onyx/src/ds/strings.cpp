#include "strings.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//http://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterator-define.html

namespace hoi
{

string :: string(char *buffer, string_size_t capacity)
{
	_buffer = buffer;
	_buffer[0] = '\0';
	
	_capacity = capacity;
	_length = 0;
}

// Default Copy Constructor
string :: string(const string &str)
{
	_buffer = str._buffer;
	_length = str._length;
	_capacity = str._capacity;
}

int string :: checkAndResize(string_size_t size)
{
	if (_capacity >= size)
		return 0;
	return -1;
}


char string :: charAt(string_size_t index) const
{
	return _buffer[index];
}

void string :: setCharAt(char c, string_size_t index)
{
	_buffer[index] = c;
}

int string :: copy(const char *cstr2, string_size_t length)
{
	if (checkAndResize(length))
		return -1;
	memcpy(_buffer, cstr2, length);
	_buffer[length] = '\0';
	_length = length;
	return 0;
}

int string :: copy(const char *cstr2)
{
	string_size_t length = strnlen(cstr2, MAX_STRING_LENGTH);
	return copy(cstr2, length);
}

int string :: copy(const string &str)
{
	if (!(str.isValid()))
		return -1;
	return copy(str._buffer, str._length);
}


int string :: concat(const char *cstr, string_size_t length)
{
	if (checkAndResize(_length + length))
		return -1;
	strncat(_buffer + _length, cstr, length);
	_length += length;
	return 0;
}

int string :: concat(const char *cstr)
{
	string_size_t length = strnlen(cstr, MAX_STRING_LENGTH);
	return concat(cstr, length);
}

int string :: concat(const char c)
{
	char cstr2[2];
	cstr2[0] = c;
	cstr2[1] = '\0';
	return concat(cstr2, 2);
}

int string :: concat(const string &str)
{
	if (!(str.isValid()))
		return -1;
	return concat(str._buffer, str._length);
}

void string :: toLowerCase(void)
{
	if (!isValid())
		return; 

	string_size_t i;
	for (i=0; i<_length; i++)
		_buffer[i] = tolower(_buffer[i]);
}

int string :: indexOf(char ch) const
{
	return indexOf(ch,0);
}

int string :: indexOf(char ch, unsigned fromIndex) const
{
	if (!isValid())
		return -1;

	string_size_t i;
	for (i=fromIndex; i<_length; i++)
		if (_buffer[i] == ch)
			return i;
	return -1;
}

int string :: lastIndexOf(char ch) const
{
	return lastIndexOf(ch,_length-1);
}

void string :: replace(char find, char replace)
{
	if (!isValid())
		return;

	string_size_t i;
	for (i=0; i<_length; i++)
		if (_buffer[i] == find)
			_buffer[i] = replace;
}

int string :: lastIndexOf(char ch, unsigned fromIndex) const
{
	if (!isValid())
		return -1;

	string_size_t i;
	for (i=fromIndex; i>=0; i--)
		if (_buffer[i] == ch)
			return i;
	return -1;
}


void string :: toUpperCase(void)
{
	if (!isValid())
		return;

	string_size_t i;
	for (i=0; i<_length; i++)
		_buffer[i] = toupper(_buffer[i]);
}

void string :: trim(void)
{
	if (!isValid())
		return;

	string_size_t i;
	for (i=0; (isspace(_buffer[i]) and i<_length); i++);

	memcpy(_buffer, _buffer + i, _length - i);
	_length -= (i - 1);

	for (; (isspace(_buffer[_length]) and _length > 0); _length--);
	_length++;
	_buffer[_length] = '\0';
}


int string :: compareTo(const string &str2)
{
	if (!_buffer || !str2._buffer) {
		if (_buffer && (_length > 0)) return *static_cast<char *>(_buffer);
		if (str2._buffer && (str2._length > 0)) return (0 - *static_cast<char *>(str2._buffer));
		return 0;
	}
	return strncmp(_buffer, str2._buffer, MAX_STRING_LENGTH);
}

int string :: compareTo(const char *cstr2)
{
	if (!cstr2 || !_buffer) {
		if (cstr2) return (0 - *const_cast<char *>(cstr2));
		if (_buffer) return *const_cast<char *>(_buffer);
	}
	return strncmp(_buffer, cstr2, MAX_STRING_LENGTH);
}

int string :: compareTo(const char *cstr2, string_size_t length)
{
	if (!cstr2 || !_buffer) {
		if (cstr2) return (0 - *const_cast<char *>(cstr2));
		if (_buffer) return *const_cast<char *>(_buffer);
	}
	return strncmp(_buffer, cstr2, MAX_STRING_LENGTH);
}

int string :: toInt(int &i) const
{
	if (sscanf(_buffer, "%d", &i) == 1)
		return 0;
	return -1;
}

int string :: toFloat(float &f) const
{
	// http://stackoverflow.com/questions/905928/using-floats-with-sprintf-in-embedded-c
	if (sscanf(_buffer, "%f", &f) == 1)
		return 0;
	return -1;
}

} 


//#define __MAIN__

#ifdef __MAIN__

#define MAX_BUF_SIZE 50
	
int main()
{
	//STRING_DEF(str, 20);
	char buf[MAX_BUF_SIZE] = "ABHISHEK";
	hoi :: string a(buf, MAX_BUF_SIZE);
	a.copy("  ABHISHEK   ");
	printf("String Len - %d\n", a.size());
	printf("String: %s\n", a.c_str());
	a.toLowerCase();
	printf("String After Lower Case: %s\n", a.c_str());
	a.trim();
	printf("String After trim: %s\n", a.c_str());
	printf("Index of i: %d\n", a.indexOf('i'));

	return 0;
}
#endif
