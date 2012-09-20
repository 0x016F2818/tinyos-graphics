#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"
#include "http_parser.h"


#define TGX_HTTP_RESPONSE_STATUS_MAP(XX)						\
	XX(0, 200, HTTP/1.1 200 OK)								\
	XX(1, 304, HTTP/1.1 304 Not Modified)					\
	XX(2, 400, HTTP/1.1 400 Bad Request)					\
	XX(3, 403, HTTP/1.1 403 Forbidden)						\
	XX(4, 404, HTTP/1.1 404 Not Found)						\
	XX(5, 500, HTTP/1.1 500 Internal Server Error)			\
	XX(6, 505, HTTP/1.1 505 HTTP Version not supported)		\

#undef NULL
#define NULL ((void *)0)

typedef enum tgx_http_status {
#define XX(num, name, string) TGX_HTTP_STATUS_##name = num,
	TGX_HTTP_RESPONSE_STATUS_MAP(XX)
#undef XX
} tgx_http_status_t;

static const char *tgx_http_status_string[] = {
#define XX(num, name, string) #string,
	TGX_HTTP_RESPONSE_STATUS_MAP(XX)
	NULL,
#undef XX
};

#define TGX_MIME_TYPE_MAP(XX)		  			\
	XX(0, HTML,         text/html)			   	\
	XX(1, CSS,          text/css )				\
	XX(2, JAVASCRIPT,   text/javascript)		\
	XX(3, JPEG,         image/jpeg)				\
	XX(4, JPG,          image/jpg)				\
	XX(5, PNG,          image/png)				\
	XX(6, GIF,          image/gif)				\
	XX(7, ICO,          image/ico)				\
	XX(8, PLAIN,        text/plain)				\

typedef enum tgx_mime_type {
#define XX(num, name, string) TGX_MIME_TYPE_##name = num,
	TGX_MIME_TYPE_MAP(XX)
#undef XX
} tgx_mime_type_t ;

static const char *tgx_mime_type_string[] = {
#define XX(num, name, string) #string,
	TGX_MIME_TYPE_MAP(XX)
	NULL,
#undef XX
};

tgx_mime_type_t tgx_get_mime_type(char *str);

int tgx_http_parser_on_url_cb(http_parser *parser, const char *at, size_t length);
int tgx_http_parser_on_header_field_cb(http_parser *parser, const char *at, size_t length);
int tgx_http_parser_on_header_value_cb(http_parser *parser, const char *at, size_t length);
int tgx_http_parser_on_header_complete_cb(http_parser *parser);
int tgx_http_parser_on_body_cb(http_parser *parser, const char *at, size_t length);
