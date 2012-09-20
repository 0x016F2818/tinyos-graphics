#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"

struct tgx_connection_s {
#define TGX_NO_VALUE			-1
#define TGX_MAX_HEADERS			128
#define TGX_MAX_ELEMENT_SIZE	1024
#define TGX_MAX_PATH_LENGTH		4096
	int									fd;

	// 打算GET和POST都写2个writer
	// writer_header
	// writer_body, GET的writer_body发送文件, POST的writer_body发送模块产生的数据
	// 然后写完一部分就改变指针, 进入下一个环节
	tgx_string_t						*httpReqBuf;
	tgx_string_t						*httpReqBody;
	tgx_string_t						*httpRespHeader;
	tgx_string_t						*httpRespBody;

	pthread_mutex_t						*lock;

	int									rw_pos;
	struct tgx_http_parser_s {
		int     http_status;
		int		mime_type;
		char	path[4096];
		char	http_method[16];
		int		content_length;
		int num_headers;
		enum { 
			NONE = 0, 
			FIELD, 
			VALUE 
		} last_header_element;
		char headers[TGX_MAX_HEADERS][2][TGX_MAX_ELEMENT_SIZE];

		enum {
			CGI_MODULE = 1,
			WSN_MODULE,
		} mod_flag;
	} *http_parser;

	void							*dlopen_handle;
};

tgx_connection_t *tgx_new_connection(tgx_cycle_t *tcycle, int fd);
void tgx_free_connection(tgx_connection_t *tconn);
int tgx_connection_handler_read             (tgx_cycle_t *tcycle, void *context, int event);
void tgx_connection_handler_close           (tgx_cycle_t *tcycle, void *context, int event);
