#ifndef _CSTRING_H_
#define _CSTRING_H_

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_STRING_LENGTH			512

#define STRING_DEF(s,c)			char s##_buf[c]; \
									hoi :: string s(s##_buf, c);

namespace hoi 
{

typedef unsigned string_size_t;

class string {
private:
	int checkAndResize(string_size_t size);

protected:
	char *_buffer;
	string_size_t _capacity;
	string_size_t _length;

public:
	explicit string(char *buffer, string_size_t capacity);
	explicit string(const string &str);

	string_size_t size(void) const { return _capacity; }
	string_size_t length(void) const { return _length; }
	char *c_str() const { return _buffer; }

	bool isValid(void) const { return (_buffer and (_length > 0)); }

	void clear(void) { _length = 0; _buffer[0] = '\0'; }

	char charAt(string_size_t index) const;
	void setCharAt(char c, string_size_t index);

	int copy(const string &str2);
	int copy(const char *cstr2, string_size_t length);
	int copy(const char *cstr2);

	int concat(const char *cstr, string_size_t length);
	int concat(const char *cstr);
	int concat(const char c);
	int concat(const string &str);

	int indexOf(char ch) const;
	int indexOf(char ch, unsigned fromIndex) const;

	int lastIndexOf(char ch) const;
	int lastIndexOf(char ch, unsigned fromIndex) const;

	void replace(char find, char replace);

	void toLowerCase(void);
	void toUpperCase(void);

	void trim(void);

	int compareTo(const string &str2);
	int compareTo(const char *cstr2);
	int compareTo(const char *cstr2, string_size_t length);

	int toInt(int &i) const;
	int toFloat(float &f) const;
};

}


#endif
