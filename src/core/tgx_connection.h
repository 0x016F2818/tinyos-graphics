#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"

struct tgx_connection_s {
	int									fd;

	// 状态机中的状态， 不是http的状态， 所有与http有关的解析信息全在http_parser中
	int									status;

	tgx_string_t						*buffer;
	int									read_pos;
	int									write_pos;
	tgx_http_parser_t					*http_parser;
};

tgx_connection_t *tgx_connection_init(tgx_cycle_t *tcycle, int fd);
void tgx_connection_free(tgx_connection_t *tconn);

// 这里可以注册多个handler， 我们使用状态机在多个handler之间切换
typedef int (tgx_connection_handler_t)(tgx_cycle_t *tcycle, void *context, int event);

tgx_connection_handler_t tgx_connection_read_req_header,
						 tgx_connection_parse_req_header,
						 tgx_connection_get_send_resp_header,
						 tgx_connection_get_send_resp_file,
						 tgx_connection_post_send_resp_header,
						 tgx_connection_post_read_req_message_body,
						 tgx_connection_post_send_resp_data,
						 tgx_connection_close,
						 tgx_connection_error;
