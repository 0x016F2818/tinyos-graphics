#include <stdio.h>
#include "tgx_module.h"
#include <mysql.h>
#include <stdlib.h>
#include <json/json.h>
#include "sensor.h"
#include <string.h>

int TGX_MODULE_HTTP_HANDLER(tgx_module_http_t *http)
{
	db_connect_info_t db_connect_info;

	struct json_object *request; /* request information point json_object */
	/* struct json_object *template; /\* template information point json_object *\/ */
	struct json_object *respons; /* respons information point json_object */

	request = json_tokener_parse( http->req->data );

	/* get the message from request json_object */
	char *net_name = (char *)json_object_get_string( json_object_object_get( request, "net_name" ) );
	int   nodeId   = json_object_get_int( json_object_object_get( request, "nodeId" ) );
	char *sense = (char *)json_object_get_string( json_object_object_get( request, "sense" ) );
	char *start_time = (char *)json_object_get_string( json_object_object_get( request, "start_time" ) );
	char *end_time = (char *)json_object_get_string( json_object_object_get( request, "end_time" ) );

    struct json_object *new_obj;
    http->resp->data = calloc(65536000, sizeof(char));
    http->resp->size = 65536000;
    MYSQL mysql;
    
    sensor_t sensor_info[1000000];
    memset(sensor_info, 0, sizeof(sensor_info));
    strcpy(db_connect_info.host,"10.18.46.111");
    strcpy(db_connect_info.user,"tinyos");
    strcpy(db_connect_info.password,"tinyos");
    strcpy(db_connect_info.db_name,"tinyos");

    if(get_db_handler(&mysql,db_connect_info) == -1)
        return -1;

    long i;
    long nSense_Record;
    nSense_Record = get_absolute_record(&mysql,sensor_info);
    printf("nSense_Record = %ld\n", nSense_Record);
    if (nSense_Record < 0) return -1;

    json_object *responsArray = json_object_new_array();
    for( i = 0; i < nSense_Record; i++ ) {
        json_object_object_add(respons, "time", json_object_new_string(info->time));
        json_object_object_add(respons, "value", json_object_new_double(info->temp));
        json_object_array_add(responsArray, respons);
    }
    sprintf(http->resp->data, "%s\n", json_object_to_json_string(responsArray));

	return 0;
}
