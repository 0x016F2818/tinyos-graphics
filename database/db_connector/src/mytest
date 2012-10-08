#include "json/json.h"
#include "../include/sensor.h"
#include "../include/tgx_module.h"

int TGX_MODULE_HTTP_HANDLER(tgx_module_http_t *http)
{
	struct json_object *new_obj;
	http->resp->data = calloc(1024, sizeof(char));
	http->resp->size = 1024;
	new_obj = json_tokener_parse("{						\
		  \"1\":										\
		  {												\
		  \"liupeng\": {								\
				\"other name\": \"van9ogh\",			\
				\"age\"		  : 23,						\
				\"love\"      : \"computer science\"	\
				} 										\
		 },												\
		  \"2\":										\
		 {												\
		  \"hulang\": {									\
				\"other name\": \"hu\",					\
				\"age\"		  : 23,						\
				\"love\"      : \"computer science\"	\
				} 										\
		 },												\
		  \"3\":										\
		 {												\
		  \"lijunqian\": {								\
				\"other name\": \"lidahai\",			\
				\"age\"		  : 23,						\
				\"love\"      : \"computer science\"	\
				} 										\
		 },												\
		  \"4\":										\
		 {												\
		  \"nijunjia\": {								\
				\"other name\": \"junjia\",				\
				\"age\"		  : 23,						\
				\"love\"      : \"computer science\"	\
				} 										\
		 },												\
  }");
	sprintf(http->resp->data, "%s", json_object_to_json_string(new_obj));

	return 0;
}
