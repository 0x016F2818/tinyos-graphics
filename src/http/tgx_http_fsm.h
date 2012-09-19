#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"


#define TGX_STATUS_READ_REQUEST
#define TGX_STATUS_PARSING_REQUEST
#define TGX_STATUS_SEND_RESPONSE

// 一旦发生错误无条件close
#define TGX_STATUS_CLOSE

// 这个状态可以支持keep-alive, 在send response完成后
// 可以不关闭connection
#define TGX_STATUS_KEEPALIVE
