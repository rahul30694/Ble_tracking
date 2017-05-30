#ifndef _HTTP_COMMON_H_
#define _HTTP_COMMON_H_

#include <stdint.h>
#include <stdlib.h>

/* HTTP Version Number */
#define HTTP_1_1					11
#define HTTP_1_0					10
#define HTTP_2_0					20
/* End */

#define HTTP_MESSAGE_MAX_LEN		512
#define HTTP_MESSAGE_MIN_LEN		18
#define HTTP_MAX_CONTENT_LEN		1024
#define HTTP_CHUNK_MIN_LEN			5

#define HTTP_MAX_HEADER_NUM 		20
#define HTTP_MAX_METHODS			HTTP_METHODS_SENTIMEL

#define isURLchr(c)			(((c >= '&') && (c <= ';')) || (c == '=') || ((c >= '?') && (c <= '[')) || \
 							(c == ']') || (c == '_') || islower(c) || (c == '~')  || \
 							(c == '!') || (c == '#') || (c == '$'))

#define IS_HEADER_CHAR(ch)                                                     \
  (ch == CR || ch == LF || ch == 9 || ((unsigned char)ch > 31 && ch != 127))


enum http_status_codes_e {
	
	CONTINUE=100,
	SWITCHING_PROTOCOL,

	INFORMATIONAL_SENTINEL,

	OK=200,
	CREATED,
	ACCEPTED,
	NON_AUTHORITATIVE_INFO,
	NO_CONTENT,
	RESET_COUNTER,
	PARTIAL_CONTENT,

	SUCCESSFUL_SENTINEL,

	MULTIPLE_CHOICES=300,
	MOVED_PERMANENTLY,
	FOUND,
	SEE_OTHER,
	NOT_MODIFIED,
	USE_PROXY,
	UNUSED,
	TEMPORARY_REDIRECT,

	REDIRECTION_SENTINEL,

	BAD_REQUEST=400,
	UNAUTHORIZED,
	PAYMENT_REQUIRED,
	FORBIDDEN,
	NOT_FOUND,
	METHOD_NOT_ALLOWED,
	NOT_ACCEPTABLE,
	PROXY_AUTHENTICATION_REQUIRED,
	REQUEST_TIMEOUT,
	CONFLICT,
	GONE,
	LENGTH_REQUIRED,
	PRECONDITION_FAILED,
	ENTITY_TOO_LARGE,
	URL_TOO_LONG,
	USUPPORTED_MEDIA,
	RANGE_NOT_SATISFIABLE,
	EXPECTATION_FAILED,

	CLIENT_SENTINEL,

	SERVER_ERROR=500,
	NOT_IMPLEMENTED,
	BAD_GATEWAY,
	SERVICE_UNAVAILABLE,
	GATEWAY_TIMEOUT,
	VERSION_NOT_SUPPORTED,

	SERVER_SENTINEL
};

#define HTTP_METHOD_MAP(XX)		\
	XX(0, GET, GET)				\
	XX(1, HEAD, HEAD)			\
	XX(2, POST, POST)			\
	XX(3, PUT, PUT)				\
	XX(4, DELETE, DELETE)		\
	XX(5, CONNECT, CONNECT)		\
	XX(6, TRACE, TRACE)			



struct http_header_s {
	const char *const name;
	
	int (*validator)(struct http_message_s *m, const char *val, unsigned len);
};

struct http_message_s {
	uint8_t type; // Request or Response

	/* Initial Line (Request or Response) */
	char version;
	union {
		uint16_t status_code; 	//For Response
		uint8_t method; 		//For Request
	};
	/* Initial Line End */

	union {
		uint16_t flags;
		struct {
			uint8_t f_chunked:1;
			uint8_t f_idenity:1;
			uint8_t f_trailing:1;

			uint8_t f_con_upgrade:1;
			uint8_t f_con_alive:1;
			uint8_t f_con_close:1;

			uint8_t f_upgrade:1;
			uint8_t f_no_body:1;
			uint8_t f_content_len:1;

			uint8_t f_message_complete:1;
		};
	};

	/* Entity Body */
	const void *body;
	uint16_t body_length;
	/* End */

	/* Chunked Encoding Total Length */
	int16_t total_chunked_len;
};


typedef int (*http_data_callback_t) (struct http_message_s *m, const char *val, size_t length);
typedef int (*http_header_callback_t) (struct http_message_s *m, unsigned header, const char *val, size_t length);
typedef void (*http_notify_callback_t) (struct http_message_s *m);

struct http_parser_settings_s {
	http_data_callback_t on_url;
	http_data_callback_t on_status;

	http_header_callback_t on_header;
	http_data_callback_t on_body;
	http_data_callback_t on_chunk;

	http_notify_callback_t on_complete;
};


extern const struct http_header_s http_headers[];
extern const char *const http_method[];

enum http_methods_e {
#define XX(num,name,cstr)	name,
	HTTP_METHOD_MAP(XX)
#undef XX
	HTTP_METHODS_SENTINEL
};

enum con_values_e {
	CON_KEEP_ALIVE = 0,
	CON_CLOSE,
	CON_UPGRADE,
	CON_VAL_SENTINEL
};

enum te_values_e {
	TE_CHUNKED = 0,
	TE_INDENITY,
	TE_GZIP,
	TE_COMPRESS,
	TE_DEFALTE,
	TE_VAL_SENTINEL
};


enum http_headers_e {
	HTTP_CONNECTION=0,
	HTTP_DATE,
	HTTP_TRANSFER_ENCODING,
	HTTP_UPGRADE,

	HTTP_CONTENT_LENGTH,
	HTTP_CONTENT_ENCODING,
	HTTP_CONTENT_MD5,
	HTTP_CONTENT_RANGE,
	HTTP_CONTENT_TYPE,

	HTTP_TE,
	HTTP_USER_AGENT,
	HTTP_FROM,
	HTTP_HOST,

	HTTP_RETRY_AFTER,
	HTTP_SERVER,
	HTTP_HEADERS_SENTINEL
};

enum http_message_e {
	HTTP_INVALID=0, HTTP_REQUEST, HTTP_RESPONSE
};

enum http_conn_e {
	CLOSE, KEEP_ALIVE, UPGRADE
};

/* HTTP Parser Specific */
struct http_parser_s {
	const char *data;
	int16_t length;
	int16_t index;

	uint8_t state;

	struct http_message_s message;
};


int http_parser_init(struct http_parser_s *p);
int http_parser_settings_init(struct http_parser_settings_s *s);
int http_parse(struct http_parser_s *p, struct http_parser_settings_s *s, const char *data, size_t len);


/* HTTP Former Specific */
struct http_former_s {
	unsigned length;

	// HTTP Formed Message BreakDown
	struct http_message_s message;
};

int http_former_init(struct http_former_s *f, uint8_t version, uint8_t type);
int http_print_request_line(struct http_former_s *f, char *data, uint8_t method, const char *url=NULL, bool validate=false);

int http_print_status_line(struct http_former_s *f, char *data, int status_code, char *message, bool validate=false);
int http_print_header(struct http_former_s *f, char *data, uint8_t header, char *value, bool validate=false);

int http_print_header(struct http_former_s *f, char *data, uint8_t header, int value, bool validate=false);
int http_header_end(struct http_former_s *f, char *data);

int http_append_normal_body(struct http_former_s *f, char *data, const char *body, unsigned length, bool validate=false);
int http_append_chunk_body(struct http_former_s *f, char * chunk, char *data, unsigned length, bool validate=false);



/*struct http_header_s http_header[HEADER_SENTINEL] = {
#ifndef HTTP_LIGHT_WEIGHT
	[CACHE_CONTROL] 			= 	{	"Cache-Control",		 0, 	0,		NULL},
#endif
	[CONNECTION] 				= 	{	"Connection", 			 0, 	0,		on_connection},
	[DATE] 						= 	{	"Date", 				 0, 	0,		on_date},
	[TRANSFER_ENCODING] 		=   {	"Transfer-Encoding",	 0,		0,		on_transfer_encoding},
	[UPGRADE]					=	{	"Upgrade",				 0,		0,		NULL},

	[GENERAL_HEADER_SENTINEL]	= 	{	NULL,				 	 0,		0,		NULL},

#ifndef HTTP_LIGHT_WEIGHT
	[ACCEPT] 					= 	{	"Accept", 		 		 0, 	0, 		NULL},
	[ACCEPT_CHARSET] 			= 	{	"Accept-Charset", 		 0, 	0,	 	NULL},
	[ACCEPT_ENCODING] 			= 	{	"Accept-Encoding", 		 0, 	0, 		NULL},
	[ACCEPT_LANGUAGE] 			= 	{	"Accept-Language", 		 0, 	0,	 	NULL},
	[EXPECT]					=	{	"Expect",		 		 0,		0,		NULL},
	[IF_MATCH]					=	{	"If-Match",		 		 0,		0,		NULL},
	[IF_MODIFIED_SINCE]			=	{	"If-Modified-Since",	 0,		0,		NULL},
	[IF_MATCH_NONE]				=	{	"If-Match-None",		 0,		0,		NULL},
	[IF_RANGE]					=	{	"If-Range",		 		 0,		0,		NULL},
	[IF_UNMODIFIED_SINCE]		=	{	"If-Unmodified-Since",	 0,		0,		NULL},
	[REFERER]					=	{	"Referer",		 		 0,		0,		NULL},
	[MAX_FORWARDS] 				= 	{	"Max-Forwards", 		 0, 	0,	 	NULL},
#endif
	[TE] 						= 	{	"TE", 					 0, 	0,		NULL},
	[USER_AGENT] 				= 	{	"User-Agent", 			 0, 	0,		NULL},
	[FROM]						=	{	"From",		 			 0,		0,		NULL},
	[HOST]						=	{	"Host",		 			 0,		0,		NULL},
	[AUTHORIZATION]				=	{	"Authorization",		 0,		0,		NULL},
	[PROXY_AUTHORIZATION]		=	{	"Proxy-Authorization",	 0,		0,		NULL},
	[RANGE]						=	{	"Range",		 		 0,		0,		NULL},
	

	[REQUEST_HEADER_SENTINEL] 	= 	{	NULL, 					 0, 	0, 		NULL},

#ifndef HTTP_LIGHT_WEIGHT
	[ACCEPT_RANGES]		 		= 	{	"Accept-Ranges", 		 0, 	0,		NULL},
	[VARY]						=	{	"Vary",			 		 0,		0,		NULL},
	[WWW_AUTHENTICATE]			=	{	"WWW-Authenticate",		 0,		0,		NULL},
	[ETAG] 						= 	{	"ETag", 		 		 0, 	0,		NULL},
#endif
	[PROXY_Authenticate] 		= 	{	"Proxy-Authenticate", 	 0, 	0,		NULL},
	[RETRY_AFTER] 				=   {	"Retry-After",	 		 0,		0,		NULL},
	[SERVER]					=	{	"Server",				 0, 	0,		NULL},
	[AGE] 						= 	{	"Age", 		 			 0, 	0,	 	NULL},
	[LOCATION] 					= 	{	"Location", 			 0, 	0,		NULL},

	[RESPONSE_HEADER_SENTINEL] 	= 	{ 	NULL, 					 0, 	0, 		NULL},

#ifndef HTTP_LIGHT_WEIGHT
	[ALLOW]		 				= 	{	"Allow", 				 0, 	0,		NULL},
	[CONTENT_ENCODING] 			= 	{	"Content-Encoding", 	 0, 	0,	 	NULL},
	[CONTENT_LOCATION] 			= 	{	"Content-Location", 	 0, 	0,		NULL},
	[CONTENT_LANGUAGE] 			= 	{	"Content-Language", 	 0, 	0,		NULL},
	[LAST_MODIFIED]				=	{	"Last-Modified",		 0,		0,		NULL},
	[EXTENSION_HEADER]			=	{	"extension-header",		 0,		0,		NULL},
#endif
	[CONTENT_LENGTH] 			= 	{	"Content-Length", 		 0, 	0,		NULL},
	[CONTENT_MD5] 				=   {	"Content-MD5",	 		 0,		0,		NULL},
	[CONTENT_RANGE]				=	{	"Content-Range",		 0, 	0, 		NULL},
	[CONTENT_TYPE]				=	{	"Content-Type",			 0,		0,		NULL},
	[EXPIRES]					=	{	"Expires",			 	 0,		0,		NULL},
};
*/


#endif