#include "http_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*struct http_parser_settings {
  http_cb      on_message_begin;
  http_data_cb on_url;
  http_data_cb on_header_field;
  http_data_cb on_header_value;
  http_cb      on_headers_complete;
  http_data_cb on_body;
  http_cb      on_message_complete;
};*/

#define MAX_HEADERS 100
#define MAX_ELEMENT_SIZE 1000

struct connection {
	int num_headers;
	enum { NONE=0, FIELD, VALUE } last_header_element;
	char headers [MAX_HEADERS][2][MAX_ELEMENT_SIZE];
};


int on_url_cb(http_parser *parser, const char *at, size_t length)
{

	printf("\non_url_cb\n\t");
	int i;
	for (i = 0; i < length; i++)
		printf("%c", at[i]);
	printf("%s\n", at);
	printf("\n");
	return 0;
	}

int on_header_field_cb(http_parser *parser, const char *at, size_t length)
{

	struct connection *conn= parser->data;
	if (conn->last_header_element != FIELD)
		conn->num_headers++;

	strncat(conn->headers[conn->num_headers-1][0], at, length);
	conn->last_header_element = FIELD;

	printf("\non_header_field_cb\n\t");
	int i;
	for (i = 0; i < length; i++)
		printf("%c", at[i]);
	printf("\n");
	return 0;
}


int on_header_value_cb(http_parser *parser, const char *at, size_t length)
{

	struct connection *conn= parser->data;
	if (conn->last_header_element != FIELD)
		conn->num_headers++;
	strncat(conn->headers[conn->num_headers-1][1], at, length); 
	conn->last_header_element = VALUE;

	printf("\non_header_value_cb\n\t");
	int i;
	for (i = 0; i < length; i++)
		printf("%c", at[i]);
	printf("\n");
	return 0;
}
int on_body_cb(http_parser *parser, const char *at, size_t length)
{

	printf("\non_body_cb\n\t");
	int i;
	for (i = 0; i < length; i++)
		printf("%c", at[i]);
	printf("\n");
	return 0;
}
int on_message_begin_cb(http_parser *parser)
{

	printf("\non_message_begin_cb\n\t");
	return 0;
}
int on_headers_complete_cb(http_parser *parser)
{
	
	printf("\non_headers_complete_cb\n\t");
	return 0;

}
int on_message_complete_cb(http_parser *parser)
{

	printf("\non_message_complete_cb\n\t");
	return 0;
}


int main(int argc, char *argv[])
{

	// 1. 定义一些消息, 用来传给http_parser解析
	// START
	static char message[] = "HTTP/1.1 200 OK\r\nDate: Thu, 30 Aug 2012 08:28:27 GMT\r\nServer: Apache/2.2.22 (Ubuntu)\r\nLast-Modified: Tue, 21 Aug 2012 15:51:57 GMT\r\nETag: \"542858-158e-4c7c89b2584b3\"\r\nAccept-Ranges: bytes\r\nContent-Length: 5518\r\nVary: Accept-Encoding\r\nContent-Type: text/html\r\n\r\n";
	static char message3[] = "POST / HTTP/1.1\r\nUser-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.1 zlib/1.2.3.4 libidn/1.23 librtmp/2.3\r\nHost: 127.0.0.1:8901\r\nAccept: */*\r\n\r\nHELLO WORLD";
	static char message4[] = "GET /index.html HTTP/1.1\r\nUser-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.1 zlib/1.2.3.4 libidn/1.23 librtmp/2.3\r\nHost: localhost:8901\r\nAccept: */*\r\nContent-Length: 11\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nHELLO WORLD";
	// END


	// 2. 设置所有的回调函数
	// START
	http_parser_settings settings;
	memset(&settings, 0, sizeof(settings));
	settings.on_message_begin = on_message_begin_cb;
	settings.on_url = on_url_cb;
	settings.on_header_field = on_header_field_cb;
	settings.on_header_value = on_header_value_cb;
	settings.on_headers_complete = on_headers_complete_cb;
	settings.on_body = on_body_cb;
	settings.on_message_complete = on_message_complete_cb;
	// END
	int i;
	struct connection *conn = calloc(1, sizeof(struct connection));

	// 3. excute
	http_parser *parser = malloc(sizeof(http_parser));
	/*http_parser_init(parser, HTTP_REQUEST);*/
	/*parser->data = conn;*/
	/*http_parser_execute(parser, &settings, message3, strlen(message3));*/


	// 4. 貌似每次使用都要初始化
	http_parser_init(parser, HTTP_REQUEST);
	memset(conn, 0, sizeof(struct connection));
	parser->data = conn;

	char *buf = calloc(1, strlen(message4)+1);
	for (i = 0; i < strlen(message4); i+=10) {
		buf[i] = message4[i];
		http_parser_init(parser, HTTP_REQUEST);
		http_parser_execute(parser, &settings, buf, 10);
	}


	printf("\n");
	return 0;
}
