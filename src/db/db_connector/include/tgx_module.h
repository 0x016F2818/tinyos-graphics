#pragma once

#ifndef TGX_STRING_T
#define TGX_STRING_T
typedef struct {
	int size;
	char *data;
} tgx_string_t;
#endif

typedef struct {
	tgx_string_t *req;
	tgx_string_t *resp;
} tgx_module_http_t;

// 由用户实现
int TGX_MODULE_HTTP_HANDLER(tgx_module_http_t *http);
