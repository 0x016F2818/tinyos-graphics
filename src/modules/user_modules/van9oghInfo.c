#include <stdio.h>
#include "tgx_module.h"
/*#include <mysql.h>*/
#include <stdlib.h>
#include <json/json.h>

int TGX_MODULE_HTTP_HANDLER(tgx_module_http_t *http)
{
	http->resp->data = calloc(1024, sizeof(char));
	http->resp->size = 1024;
	sprintf(http->resp->data, "%s", "Hello, World\n");

	return 0;
}
