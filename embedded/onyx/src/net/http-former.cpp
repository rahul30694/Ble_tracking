#include "http-common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int http_former_init(struct http_former_s *f, uint8_t version, uint8_t type)
{
	if (f == NULL)
		return -1;

	struct http_message_s *m = &(f->message);
	memset(f, 0, sizeof(*f));
	f->length = 0;

	m->type = type;
	m->version = version;
	return 0;
}

int http_print_request_line(struct http_former_s *f, char *data, uint8_t method, const char *url, bool validate)
{
	struct http_message_s *m = &(f->message);
	if (m->type != HTTP_REQUEST)
		return -1;

	int appended_len=0;
	char major = m->version/10;
	char minor = m->version - major*10;

	if (url == NULL)
		appended_len = sprintf(data, "%s / HTTP/%d.%d\r\n", http_method[method], major, minor);
	else
		appended_len = sprintf(data, "%s %s HTTP/%d.%d\r\n", http_method[method], url, major, minor);

	f->length += appended_len;
	return appended_len;
}

int http_print_status_line(struct http_former_s *f, char *data, int status_code, char *message, bool validate)
{
	struct http_message_s *m = &(f->message);
	if (m->type != HTTP_RESPONSE)
		return -1;

	int appended_len=0;
	char major = m->version/10;
	char minor = m->version - major*10;

	appended_len = sprintf(data, "HTTP/%d.%d %d %s\r\n", major, minor, status_code, message);
	f->length += appended_len;
	return appended_len;
}

#define MAX_INT_VALUE_DIGITS				20

int http_print_header(struct http_former_s *f, char *data, uint8_t header, int value, bool validate)
{
	char val_str[MAX_INT_VALUE_DIGITS + 1];
	sprintf(val_str, "%d", value);
	return http_print_header(f, data, header, val_str, validate);
}

int http_print_header(struct http_former_s *f, char *data, uint8_t header, char *value, bool validate)
{
	struct http_message_s *m = &(f->message);
	int appended_len=0;

	if (validate && http_headers[header].validator)
		if (http_headers[header].validator(m, value, strlen(value)))
			return -1;

	appended_len = sprintf(data, "%s:%s\r\n", http_headers[header].name, value);
	f->length += appended_len;
	return appended_len;
}

int http_header_end(struct http_former_s *f, char *data)
{
	int appended_len = sprintf(data, "\r\n");
	f->length += appended_len;
	return appended_len;
}


int http_append_normal_body(struct http_former_s *f, char *data, const char *body, unsigned length, bool validate)
{
	struct http_message_s *m = &(f->message);
	if (!validate)
		goto APPEND_BODY;

	if (!m->f_content_len)
		return -1;
	if (m->body_length != length)
		return -1;

APPEND_BODY:
	strncat(data, body, length);
	f->length += length;
	return length;
}

int http_append_chunk_body(struct http_former_s *f, char * chunk, char *data, unsigned length, bool validate)
{
	struct http_message_s *m = &(f->message);
	if (!validate)
		goto APPEND_BODY;

	if (m->f_content_len)
		return -1;
	if (!m->f_chunked)
		return -1;
	
APPEND_BODY:
	int appended_len=0;
	if (length != 0)
		appended_len = sprintf(chunk, "%x\r\n%s\r\n", length, data);
	else
		appended_len = sprintf(chunk, "0\r\n\r\n");
	m->total_chunked_len += appended_len;
	f->length += appended_len;
	return appended_len;
}



//#define __MAIN__

#ifdef __MAIN__

#define HTTP_CSTR_LEN 1024

int main(int argc, char *argv[])
{
	char data[HTTP_CSTR_LEN + 1];

	char mess[1024] = "{\"lock\":\"on\",\"health\":\"good\"}";
	int len = strlen(mess);
	int length=0;
	char *http = data;

	struct http_former_s former;

	if (http_former_init(&former, HTTP_1_1, HTTP_REQUEST))
		return -1;
	if ((length += http_print_request_line(&former, http+length, GET)) <= 0)
		return -1;

	if ((length += http_print_header(&former, http+length, HTTP_CONTENT_TYPE, "text/html")) <= 0)
		return -1;
	
	if ((length += http_print_header(&former, http+length, HTTP_CONTENT_LENGTH, len)) <= 0)
		return -1;
	if ((length += http_header_end(&former, http+length)) <= 0)
		return -1;
	if ((length += http_append_normal_body(&former, http+length, mess, len)) <= 0)
		return -1;
	if ((length += http_append_chunk_body(&former, http+length, mess, len)) <= 0)
		return -1;

	printf("HTTP Message is: \n");
	printf("%s\n", http);
	printf("Length is: %d and %d\n", former.length, strlen(http));
	return 0;
}

#endif