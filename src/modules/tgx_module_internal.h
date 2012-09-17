#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"

typedef struct {
	tgx_string_t *req;
	tgx_string_t *resp;
} tgx_module_http_t;

int tgx_module_http_init(tgx_module_http_t *http);
void tgx_module_http_destroy(tgx_module_http_t *http);
