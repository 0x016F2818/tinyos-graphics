#include <stdio.h>
#include "tgx_module.h"
#include <mysql.h>
#include <stdlib.h>
#include <json/json.h>
#include "sensor.h"
#include <string.h>

int TGX_MODULE_HTTP_HANDLER(tgx_module_http_t *http)
{
/* int main(int argc, char *argv[]) */
/* { */
     if( http->req != NULL ) {

          struct json_object *request; /* request information point json_object */
          /* struct json_object *template; /\* template information point json_object *\/ */
          struct json_object *respons; /* respons information point json_object */
          
          request = json_tokener_parse( http->req->data );

          /* get the message from request json_object */
          char *net_name = (char *)json_object_get_string( json_object_object_get( request, "net_name" ) );
          int   nodeId   = json_object_get_int( json_object_object_get( request, "nodeId" ) );
          char *sense = (char *)json_object_get_string( json_object_object_get( request, "sense" ) );
          char *requestNumString = (char *)json_object_get_string( 
				  json_object_object_get( request, "requestNum" ) );
          char *start_time = (char *)json_object_get_string( json_object_object_get( request, "start_time" ) );
          int requestNum = atoi( requestNumString );

          /* Stroge the information to respons */
          sensor_t *info = ( sensor_t* )calloc( 65536, sizeof( sensor_t ) );
          
          /* create http->resp->data */
          http->resp->data = calloc(65536, sizeof(char));
          http->resp->size = 65536;

          /* create the mysql connect */
          MYSQL mysql;
          db_connect_info_t db_connect_info;
          int ret, i;

          strcpy(db_connect_info.host,"10.18.46.111");
          strcpy(db_connect_info.user,"tinyos");
          strcpy(db_connect_info.password,"tinyos");
          strcpy(db_connect_info.db_name,"tinyos");

          if(get_db_handler(&mysql,db_connect_info) == -1){
               return -1;
          }

          switch( requestNum ) {
          case 1:
               ret = get_latest_record( &mysql, info, net_name, nodeId, sense );
               if( ret == -1 ) {
                    return -1;
               }
               respons = json_object_new_object();
               json_object_object_add(respons, "time", json_object_new_string(info->time));
               json_object_object_add(respons, "value", json_object_new_double(info->temp));
               sprintf(http->resp->data, "%s\n", json_object_to_json_string(respons));
               break;
          case 30:
               ret = get_relative_record( &mysql, info, net_name, nodeId, sense, start_time, requestNum );
               if(ret != 30) {
                    return -1;
               }
               json_object *responsArray = json_object_new_array();
               for( i = 0; i < requestNum; i++ ) {
                    json_object_object_add(respons, "time", json_object_new_string(info->time));
                    json_object_object_add(respons, "value", json_object_new_double(info->temp));
                    json_object_array_add(responsArray, respons);
               }
               sprintf(http->resp->data, "%s\n", json_object_to_json_string(responsArray));
               break;
          default:
               break;
          }
	 }

     return 0;
}
