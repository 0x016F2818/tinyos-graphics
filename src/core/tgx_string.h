#pragma once

typedef struct {
	int len;
	char *data;
} tgx_string_t;

#define tgx_string(str) { sizeof(str-1), (char *)str }
#define tgx_null_string { 0, NULL }
