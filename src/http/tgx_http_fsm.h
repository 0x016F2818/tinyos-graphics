#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"

#define TGX_STATUS_CONNECT								1

#define TGX_STATUS_READ_REQUEST_HEADER					2
#define TGX_STATUS_PARSING_REQUEST_HEADER				3

#define TGX_STATUS_GET_SEND_RESPONSE_HEADER				4
#define TGX_STATUS_GET_SEND_RESPONSE_FILE				5

#define TGX_STATUS_POST_READ_REQUEST_MESSAGE_BODY		6
#define TGX_STATUS_POST_PARSING_REQUEST_MESSAGE_BODY	7
#define TGX_STATUS_POST_SEND_RESPONSE_HEADER			8	
#define TGX_STATUS_POST_SEND_RESPONSE_DATA				9

#define TGX_STATUS_ERROR								10
#define TGX_STATUS_CLOSE								11
void tgx_http_fsm_set_status(tgx_connection_t *tc, int status);
int tgx_http_fsm_state_machine(tgx_cycle_t *tcycle, tgx_connection_t *tc);
