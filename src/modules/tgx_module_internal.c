#include "tgx_module_internal.h"

int tgx_module_http_init(tgx_module_http_t *http)
{
#define INITIALIZE_STRING_LENGTH 1024
	if (!http) return -1;
	http->req = calloc(1, sizeof(tgx_string_t));
	http->resp = calloc(1, sizeof(tgx_string_t));
	http->req->data = calloc(INITIALIZE_STRING_LENGTH, sizeof(char));
	http->resp->data = calloc(INITIALIZE_STRING_LENGTH, sizeof(char));
	http->req->size = INITIALIZE_STRING_LENGTH;
	http->resp->size = INITIALIZE_STRING_LENGTH;
	if (http->req == NULL || http->resp == NULL || 
		http->req->data == NULL || http->resp->data == NULL) 
		return -1;
		
	return 0;
}

void tgx_module_http_destroy(tgx_module_http_t *http)
{
	if (!http) return;
	if (http->req->data)  free(http->req->data);
	if (http->resp->data) free(http->resp->data);
	if (http->req)        free(http->req);
	if (http->resp)       free(http->resp);
}
