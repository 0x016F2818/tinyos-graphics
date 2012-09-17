#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"

struct tgx_module_http_s {
	tgx_string_t *req;
	tgx_string_t *resp;
};

int tgx_module_http_init(tgx_module_http_t *http);
void tgx_module_http_destroy(tgx_module_http_t *http);
