#include "http-common.h"
#include "../util/utils.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <iostream>

#define SAFE_WHILE(x,m)		for (uint16_t safe_ndx=0; (safe_ndx < m) && (x); safe_ndx++)	

/* Strings for commonly used words */
const char HTTP_CSTR[] = "HTTP";
const char http[] = "http";
const char absURLHttp[] = "http://";
const char absURLHttps[] = "https://";

const char *const transfer_encoding_value[TE_VAL_SENTINEL] = {
	[TE_CHUNKED] 			= 		"chunked", 
	[TE_INDENITY] 			= 		"idenity",
	[TE_GZIP] 				= 		"gzip",
	[TE_COMPRESS] 			= 		"compress",
	[TE_DEFALTE] 			= 		"defalte"
};


const char *const connection_value[CON_VAL_SENTINEL] = {
	[CON_KEEP_ALIVE]		=		"keep-alive",
	[CON_CLOSE]				=		"close",
	[CON_UPGRADE]			=		"upgrade"
};

enum http_former_state_e {
	HTTP_FORMER_INITIAL_LINE,
	HTTP_FORMER_HEADER,
	HTTP_FORMER_NORMAL_BODY,
	HTTP_FORMER_CHUNK_BODY,
	HTTP_FORMER_COMPLETE,

	HTTP_FORMER_ERR
};

int on_content_length(struct http_message_s *m, const char *val, unsigned len);
int on_connection(struct http_message_s *m, const char *val, unsigned len);
int on_transfer_encoding(struct http_message_s *m, const char *val, unsigned len);


const struct http_header_s http_headers[HTTP_HEADERS_SENTINEL] = {
	/* GENERAL HEADER FIELDS */
	[HTTP_CONNECTION] 				= 	{ 	"Connection",				on_connection },	
	[HTTP_DATE] 					= 	{ 	"Date",						NULL },
	[HTTP_TRANSFER_ENCODING] 		=   { 	"Transfer-Encoding",		on_transfer_encoding },
	[HTTP_UPGRADE]					=	{ 	"Upgrade",					NULL },


	/* ENTITY HEADER FIELDS */
	[HTTP_CONTENT_LENGTH] 			= 	{	"Content-Length",			on_content_length },
	[HTTP_CONTENT_ENCODING] 		= 	{	"Content-Encoding",			NULL },
	[HTTP_CONTENT_MD5] 				=   {	"Content-MD5",				NULL },
	[HTTP_CONTENT_RANGE]			=	{	"Content-Range",			NULL },
	[HTTP_CONTENT_TYPE]				=	{	"Content-Type",				NULL },


	/* REQUEST HEADER FIELDS */
	[HTTP_TE] 						= 	{ 	"TE",						NULL },
	[HTTP_USER_AGENT] 				= 	{	"User-Agent",				NULL },
	[HTTP_FROM]						=	{	"From",						NULL },
	[HTTP_HOST]						=	{	"Host",						NULL },

	/* RESPONSE HEADER FIELDS */
	[HTTP_RETRY_AFTER] 				=   {	"Retry-After",				NULL },
	[HTTP_SERVER]					=	{	"Server",					NULL}
};


const char *const http_method[HTTP_METHODS_SENTINEL] = {
#define XX(num,name,cstr) [num] = #cstr,	
	HTTP_METHOD_MAP(XX)
#undef XX
};

enum http_state_e {
	HTTP_INITIAL_LINE,
	HTTP_HEADER,
	HTTP_BODY,

	HTTP_PARSED,
	HTTP_ERROR
};

int parse_status_line(struct http_parser_s *p, struct http_parser_settings_s *s);
int parse_request_line(struct http_parser_s *p, struct http_parser_settings_s *s);
int parse_url(struct http_parser_s *p, struct http_parser_settings_s *s);
int parse_headers(struct http_parser_s *p, struct http_parser_settings_s *s);
int parse_normal_body(struct http_parser_s *p, struct http_parser_settings_s *s);
int parse_chunked_body(struct http_parser_s *p, struct http_parser_settings_s *s);


int on_content_length(struct http_message_s *m, const char *val, unsigned len)
{	
	if (m->f_content_len)
		return -1;
	if (sscanf(val, "%d", &(m->body_length)) <= 0)
		return -1;

	m->f_content_len = 1;
	return 0;
}

int on_connection(struct http_message_s *m, const char *val, unsigned len)
{
	if (m->f_con_alive || m->f_con_upgrade || m->f_con_close)
		return -1;

	if (!strncmp(val, connection_value[CON_KEEP_ALIVE], len))
		m->f_con_alive = 1;
	else if (!strncmp(val, connection_value[CON_CLOSE], len))
		m->f_con_close = 1;
	else if (!strncmp(val, connection_value[CON_UPGRADE], len))
		m->f_con_upgrade = 1;
	else
		return -1;

	return 0;
}

int on_transfer_encoding(struct http_message_s *m, const char *val, unsigned len)
{
	if (m->f_chunked || m->f_idenity)
		return -1;

	if (!strncmp(val, transfer_encoding_value[TE_CHUNKED], len))
		m->f_chunked = 1;
	else if (!strncmp(val, transfer_encoding_value[TE_INDENITY], len))
		m->f_idenity = 1;

	return 0;
}

int http_parser_init(struct http_parser_s *p)
{
	if (p == NULL)
		return -1;

	memset(p, 0, sizeof(*p));
	p->state = HTTP_INITIAL_LINE;
	return 0;
}

int http_parser_settings_init(struct http_parser_settings_s *s)
{
	if (s == NULL)
		return -1;

	s->on_url = NULL;
	s->on_status = NULL;
	s->on_header = NULL;
	s->on_body = NULL;
	s->on_chunk = NULL;
	s->on_complete = NULL;
	return 0;
}

int http_parse(struct http_parser_s *p, struct http_parser_settings_s *s, const char *data, size_t len)
{
	struct http_message_s *m = &(p->message);
	if ((p == NULL))
		return -1;

	p->data = data;
	p->length = len;
	p->index = 0;

	switch (p->state) {
	case HTTP_INITIAL_LINE:
	{
		if (p->length < HTTP_MESSAGE_MIN_LEN)
			goto ERROR;
		if (!strncmp(p->data, HTTP_CSTR, strlen(HTTP_CSTR))) { // HTTP RESPONSE!
			if (parse_status_line(p, s))
				goto ERROR;
		} else { 	// HTTP REQUEST!
			if (parse_request_line(p, s))
				goto ERROR;
		}
		p->state = HTTP_HEADER;
		// Fall Though!
	}
	case HTTP_HEADER:
	{
		if (parse_headers(p, s))
			goto ERROR;

		p->state = HTTP_BODY;
		// Fall Though
	}
	case HTTP_BODY:
	{
		int status=0;
		if (m->f_chunked)
			status = parse_chunked_body(p,s);
		else
			status = parse_normal_body(p,s);

		if (status < 0) {
			p->state = HTTP_ERROR;
		} else if (status == 0) {
			p->state = HTTP_PARSED;
			if (s && s->on_complete)
				s->on_complete(m);
		} else {
			p->state = HTTP_BODY;
		}
		break;
	}
	default:
	{
		goto ERROR;
	}
	}

	return 0;

ERROR:
	std :: cerr << "Error!!!!!!!!" << std :: endl;
	p->state = HTTP_ERROR;
	return -1;
}


#define HTTP_VERSION_STR_MAX_LEN				8

int parse_http_version(struct http_parser_s *p, struct http_parser_settings_s *s)
{
	struct http_message_s *m = &(p->message);
	SAFE_WHILE (isblank(p->data[p->index]), (p->length - p->index))
		p->index++;	
	
	if ((p->length - p->index) < HTTP_VERSION_STR_MAX_LEN)
		return -1;

	if (strncmp(HTTP_CSTR, &p->data[p->index], strlen(HTTP_CSTR)))
		return -1;
	p->index += strlen(HTTP_CSTR);

	if (!isbacksplash(p->data[p->index++]))
		return -1;

	char major = p->data[p->index++] - '0';
	if (p->data[p->index++] != '.')
		return -1;
	char minor = p->data[p->index++] - '0';

	m->version = (char)(major*10 + minor);
	return 0;
}

#define HTTP_STATUS_NUM_DIGITS					3

int parse_status_line(struct http_parser_s *p, struct http_parser_settings_s *s)
{
	struct http_message_s *m = &(p->message);
	int16_t status_start=0, status_end=0;

	if (parse_http_version(p, s))
		goto ERROR;
	
	SAFE_WHILE (isblank(p->data[p->index]), (p->length - p->index))
		p->index++;
	if ((p->length - p->index) < HTTP_STATUS_NUM_DIGITS)
		goto ERROR;

	int i;
	for (i = 0, m->status_code = 0; (i < HTTP_STATUS_NUM_DIGITS) && isdigit(p->data[p->index]); i++)
		m->status_code = (m->status_code * 10) + (p->data[p->index++] - '0');

	if (i != HTTP_STATUS_NUM_DIGITS)
		goto ERROR;

	SAFE_WHILE (isblank(p->data[p->index]), (p->length - p->index))
		p->index++;

	status_start = p->index;
	SAFE_WHILE (!isCR(p->data[p->index]), (p->length - p->index))
		p->index++;
	status_end = p->index - 1;

	if (!isLF(p->data[++p->index]))
		goto ERROR;
	p->index++;
	if (p->length <= p->index)
		goto ERROR;

	if (s && s->on_status)
		return s->on_status(m, p->data + status_start, status_end - status_start + 1);
	return 0;

ERROR:
	return -1;
}

/*
int checkURL(int start, int end)
{
	bool absOrRel=false;
	bool default_port=true;

	char *ptr = &parser.cstr[start];

	if (strncmp(absURLHttp, ptr, strlen(absURLHttp)))
		goto RELATIVE;
	ptr += strlen(absURLHttp);
	absOrRel = true;

	ptr = strchr(ptr, '/');
	RELATIVE:
	if (*ptr != '/')
		return -1;

	if ((ptr = strchr(ptr, ':')))
		default_port = true;
	return 0;
}
*/

int parse_url(struct http_parser_s *p, struct http_parser_settings_s *s)
{
	struct http_message_s *m = &(p->message);
	int url_start = 0, url_end = 0;

	SAFE_WHILE (isblank(p->data[p->index]), (p->length - p->index)) 
		p->index++;
	
	if (p->index >= p->length)
		return -1;

	url_start = p->index;
	SAFE_WHILE (isURLchr(p->data[p->index]), (p->length - p->index)) p->index++;
	if (p->index >= p->length)
		return -1;
	url_end = p->index;

	if (!isspace(p->data[p->index]))
		return -1;

	if (s->on_url)
		return s->on_url(m, p->data + url_start, url_end - url_start);
	return 0;	
}

#define HTTP_METHOD_MIN_LEN						3

int parse_request_line(struct http_parser_s *p, struct http_parser_settings_s *s)
{
	struct http_message_s *m = &(p->message);

	SAFE_WHILE (isblank(p->data[p->index]), (p->length - p->index)) 
		p->index++;
	if ((p->length - p->index) < HTTP_METHOD_MIN_LEN)
		goto ERROR;

	int i;
	for (i=0; i<HTTP_METHODS_SENTINEL; i++) 
		if (!strncmp(http_method[i], p->data + p->index, strlen(http_method[i])))
			break;
	if (i >= HTTP_METHODS_SENTINEL)
		goto ERROR;

	if ((p->length - p->index) < strlen(http_method[i]))
		return -1;
	p->index += strlen(http_method[i]);

	if (!isblank(p->data[p->index]))
		goto ERROR;

	m->method = i & (0b1111);
	if (parse_url(p,s))
		goto ERROR;
	if (parse_http_version(p,s))
		goto ERROR;

	SAFE_WHILE (!isCR(p->data[p->index]), (p->length - p->index))
		p->index++;

	if (!isLF(p->data[++p->index]))
		goto ERROR;

	p->index++;
	if (p->length <= p->index)
		goto ERROR;
	return 0;

ERROR:
	return -1;
}


int parse_headers(struct http_parser_s *p, struct http_parser_settings_s *s)
{
	int i, j;
	bool found = false;
	struct http_message_s *m = &(p->message);

	if ((p->length - p->index) < 2)
		goto ERROR;

	for (i=0; i<HTTP_MAX_HEADER_NUM; i++) { 
		if (isCR(p->data[p->index])) { // End of Header Fields
			if (!isLF(p->data[++p->index])) 
				goto ERROR;
			p->index++;
			break;	
		}
		SAFE_WHILE (isblank(p->data[p->index]), (p->length - p->index))
			p->index++;	
		if (p->length <= p->index)
			goto ERROR;
		for (j=0, found=false; j<HTTP_HEADERS_SENTINEL; j++) {
			int def_header_len = strlen(http_headers[j].name);
			if (((p->length - p->index) >= def_header_len) && \
				!strncmp(p->data + p->index, http_headers[j].name, def_header_len)) {
				if (isblank(p->data[p->index + def_header_len]) || \
						(p->data[p->index + def_header_len] == ':')) {
					found = true;
					p->index += def_header_len;
					break;
				}
			}
		}
		SAFE_WHILE ((p->data[p->index++] != ':'), (p->length - p->index));
		SAFE_WHILE (isblank(p->data[p->index]), (p->length - p->index))
			p->index++;	

		if (p->length <= p->index)
			goto ERROR;

		uint16_t val_start = 0, val_end = 0;
		val_start = p->index;

		SAFE_WHILE (!isCR(p->data[p->index]), (p->length - p->index))
			p->index++;	
		if (p->length <= p->index)
			goto ERROR;

		val_end = p-> index;
		SAFE_WHILE (isblank(p->data[val_end]), (val_end > val_start)) val_end--;
		if (found) {
			if (http_headers[j].validator)
				http_headers[j].validator(m, p->data + val_start, val_end - val_start);
			if (s && s->on_header)
				s->on_header(m, j, p->data + val_start, val_end - val_start);
		}

		if (!isLF(p->data[++p->index])) 
			goto ERROR;
		p->index += 1;
	}
	if (i >= HTTP_MAX_HEADER_NUM)
		goto ERROR;
	return 0;

ERROR:
	std :: cerr << "Error in Header" << std :: endl;
	return -1;
}

int parse_normal_body(struct http_parser_s *p, struct http_parser_settings_s *s)
{
	struct http_message_s *m = &(p->message);
	if (!m->f_content_len)
		return 0;
	if (m->body_length >= HTTP_MAX_CONTENT_LEN)
		return -1;
	m->body = p->data + p->index;
	if (s && s->on_body)
		s->on_body(m, static_cast<const char*>(m->body), m->body_length);
	return 0;
}

int parse_chunked_body(struct http_parser_s *p, struct http_parser_settings_s *s)
{
	struct http_message_s *m = &(p->message);
REEXECUTE:
	int length=0;
	const char *chunk;

	if (m->f_content_len)
		return -1;

	if (sscanf(p->data + p->index, "%x", &length) <= 0)
		return -1;
	SAFE_WHILE (!isLF(p->data[p->index]) , (p->length - p->index)) 
		p->index++;
	p->index++;
	if (length == 0)
		goto HANDLE_TRAILER;

	chunk = p->data + p->index;
	p->index += length + 2;

	if (s && s->on_chunk)
		s->on_chunk(m, chunk, length);
	if ((p->length - p->index) >= HTTP_CHUNK_MIN_LEN)
		goto REEXECUTE;
	return 1;

HANDLE_TRAILER:
	if (m->f_trailing)
		return -1;
	return 0;
}


#define __MAIN__	



#ifdef __MAIN__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

int status_print(struct http_message_s *m, const char *val, size_t length)
{
	std :: cout << "Status: ";
	std :: cout.write(val, length);
	std :: cout << std :: endl;
	return 0;
}

int url_print(struct http_message_s *m, const char *val, size_t length)
{
	std :: cout << "URL: ";
	std :: cout.write(val, length);
	std :: cout << std :: endl;
	return 0;
}

int body_print(struct http_message_s *m, const char *val, size_t length)
{
	std :: cout << "Body: ";
	std :: cout.write(val, length);
	std :: cout << std :: endl;
	return 0;
}

int chunk_print(struct http_message_s *m, const char *val, size_t length)
{
	std :: cout << "Chunk(" << length << "): ";
	std :: cout.write(val, length);
	std :: cout << std :: endl;
	return 0;
}

void on_complete(struct http_message_s  *m)
{
	std :: cout << "-------MESSAGE COMPLETE---------" << std :: endl;
}

int header_print(struct http_message_s *m, unsigned header, const char *val, size_t length)
{
	std :: cout << http_headers[header].name << " : ";
	std :: cout.write(val, length);
	std :: cout << std :: endl;
	return 0;
}


int main(int argc, char *argv[])
{
	int sock_fd = 0, n = 0;
	char recvBuff[1500];
	struct sockaddr_in serv_addr;

	if (argc <= 2) {
		printf("Usage: <IP-Address> <Port-Number>\n");
		return -1;
	}

	memset(recvBuff, 0, sizeof(recvBuff));

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
			printf("Error: Socket cannot be openned\n");
			return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	int port_number;
	sscanf(argv[2], "%d", &port_number);
	serv_addr.sin_port = htons(port_number);

	if (inet_pton(AF_INET, 	argv[1], &serv_addr.sin_addr) <= 0) {
		printf("Error: Wrong IP Address\n");
		printf("IP addredss: %s\n", argv[1]);
		return -1;
	}

	if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) {
			printf(	"Error: Connection to Server (%s) Failed\n", argv[1]);
			return -1;
	}

	struct http_former_s former;

	char data[100 + 1];
	int length=0;
	char *http = data;
	char mess[1024] = "{\"lock\":\"on\",\"health\":\"good\"}";
	int len = strlen(mess);

	if (http_former_init(&former, HTTP_1_1, HTTP_REQUEST))
		return -1;
	if ((length += http_print_request_line(&former, http+length, GET, "/range/1024?duration=0&chunk_size=10")) <= 0)
		return -1;

	if ((length += http_print_header(&former, http+length, HTTP_HOST, "www.httpbin.org")) <= 0)
		return -1;
	if ((length += http_print_header(&former, http+length, HTTP_CONTENT_LENGTH, len)) <= 0)
		return -1;
	if ((length += http_header_end(&former, http+length)) <= 0)
		return -1;
	if ((length += http_append_normal_body(&former, http+length, mess, len)) <= 0)
		return -1;

	printf("HTTP Message is: \n");
	printf("%s\n", http);

	struct http_parser_s p;
	struct http_parser_settings_s s;
	if (http_parser_init(&p))
		return -1;
	if (http_parser_settings_init(&s))
		return -1;
	s.on_status = status_print;
	s.on_body = body_print;
	s.on_header = header_print;
	s.on_url = url_print;
	s.on_complete = on_complete;
	s.on_chunk = chunk_print;
	if (http_parse(&p, &s, http, strlen(http)))
		return -1;

	if (http_parser_init(&p))
		return -1;
	

	write(sock_fd, http, strlen(http));

	while ((n = read(sock_fd, recvBuff, sizeof(recvBuff))) > 0) {
		recvBuff[n] = 0;
		//std :: cout << recvBuff << std :: endl;

		if (http_parse(&p, &s, recvBuff, n))
			return -1;
		std :: cout << "--------Response----------" << std :: endl;

		if (fputs(recvBuff, stdout) == EOF)
			printf("Error: Read Failed\n");
	}
	if (n < 0) 
		printf("Error: Read Failed\n");

	return 0;
}
#endif
