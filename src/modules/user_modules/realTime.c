#include <stdio.h>
#include "tgx_module.h"
#include <mysql.h>
#include <stdlib.h>
#include <json/json.h>
#include "sensor.h"
#include <string.h>
#include <time.h>

int TGX_MODULE_HTTP_HANDLER(tgx_module_http_t *http)
{
     printf( "???????????????????????????????????????????????????????????????" );
     printf( "???????????????????????????????????????????????????????????????" );
/* int main(int argc, char *argv[]) */
/* { */
     char start_time[ 200 ];
     time_t t;
     struct tm *tmp;
     MYSQL mysql;

     if( http->req != NULL ) {

          struct json_object *request; /* request information point json_object */
          /* struct json_object *template; /\* template information point json_object *\/ */
          struct json_object *respons = json_object_new_object(); /* respons information point json_object */
          json_object *responsArray = json_object_new_array();

          printf("\n|%s|,,,strlen(data) = %d\n", http->req->data, strlen(http->req->data));
          int k, len = strlen(http->req->data);
          for (k = 0; k < len; k++)
               printf("%c ", http->req->data[k]);

          printf(".......\n");
          request = json_tokener_parse( http->req->data );

          if (request == NULL) {
               printf("Hello World\n");
               goto err;
               return -1;
          }

          /* get the message from request json_object */
          char *net_name = (char *)json_object_get_string( json_object_object_get( request, "net_name" ) );
          int   nodeId   = json_object_get_int( json_object_object_get( request, "nodeId" ) );
          char *sense = (char *)json_object_get_string( json_object_object_get( request, "sense" ) );
          char *requestNumString = (char *)json_object_get_string( 
               json_object_object_get( request, "requestNum" ) );
          /* char *start_time = (char *)json_object_get_string( json_object_object_get( request, "start_time" ) ); */
		  
          int requestNum = atoi( requestNumString );

          /* Stroge the information to respons */
          sensor_t *info = ( sensor_t* )calloc( 65536, sizeof( sensor_t ) );
		  
          /* create http->resp->data */
          http->resp->data = calloc(65536, sizeof(char));
          http->resp->size = 65536;

          /* create the mysql connect */
          int ret, i;

          if(get_db_handler(&mysql) == -1){
               goto err;
               return -1;
          }

          t = time( NULL );
          tmp = localtime( &t );
          if( tmp == NULL ) {
               goto err;
               return -1;
          }
          if( strftime( start_time, sizeof( start_time ), "%Y%m%d%H%M%S", tmp ) == 0 ) {
               goto err;
               return -1;
          }
          printf("sensn = %s, start time = %s\n", sense, start_time);
          //////////////////////////////////////////////////////////////////////////////
          requestNum = -requestNum;
          ret = get_relative_record( &mysql, info, net_name, nodeId, sense, start_time, requestNum );
          printf("ret = %d\n", ret);
          if(ret < 0) {
               goto err;
               return -1;
          }
          requestNum = -requestNum;

          for( i = 0; i < requestNum; i++ ) {
               respons = json_object_new_object();
               json_object_object_add(respons, "time", json_object_new_string(info[i].time));
               json_object_object_add(respons, "value", json_object_new_double(info[i].temp));
               json_object_array_add(responsArray, respons);
          }
          sprintf(http->resp->data, "%s\n", json_object_to_json_string(responsArray));
          printf("%s\n", json_object_to_json_string(responsArray));

          /* switch( requestNum ) { */
          /* case 1: */
          /*      /\* ret = get_latest_record( &mysql, info, net_name, nodeId, sense ); *\/ */
          /*      /\* printf("ret = %d, info[0].time = %s, info[0].temp = %lf\n", ret, info[0].time, info[0].temp); *\/ */
          /*      /\* if( ret == -1 ) { *\/ */
          /*      /\*      goto err; *\/ */
          /*      /\*      return -1; *\/ */
          /*      /\* } *\/ */
          /*      /\* respons = json_object_new_object(); *\/ */
          /*      /\* json_object_object_add(respons, "time", json_object_new_string(info[0].time)); *\/ */
          /*      /\* json_object_object_add(respons, "value", json_object_new_double(info[0].temp)); *\/ */
          /*      /\* json_object_array_add(responsArray, respons); *\/ */
          /*      /\* sprintf(http->resp->data, "%s\n", json_object_to_json_string(responsArray)); *\/ */
          /*      /\* break; *\/ */


          /*      /////////////////////////////////////////////////////////////////////////// */
          /* case 30: */
          /*      t = time( NULL ); */
          /*      tmp = localtime( &t ); */
          /*      if( tmp == NULL ) { */
          /*           goto err; */
          /*           return -1; */
          /*      } */
          /*      if( strftime( start_time, sizeof( start_time ), "%Y%m%d%H%M%S", tmp ) == 0 ) { */
          /*           goto err; */
          /*           return -1; */
          /*      } */
          /*      printf("sensn = %s, start time = %s\n", sense, start_time); */
          /*      ////////////////////////////////////////////////////////////////////////////// */
          /*      requestNum = -requestNum; */
          /*      ret = get_relative_record( &mysql, info, net_name, nodeId, sense, start_time, requestNum ); */
          /*      printf("ret = %d\n", ret); */
          /*      if(ret != -1) { */
          /*           goto err; */
          /*           return -1; */
          /*      } */
          /*      requestNum = -requestNum; */

          /*      for( i = 0; i < requestNum; i++ ) { */
          /*           respons = json_object_new_object(); */
          /*           json_object_object_add(respons, "time", json_object_new_string(info[i].time)); */
          /*           json_object_object_add(respons, "value", json_object_new_double(info[i].temp)); */
          /*           json_object_array_add(responsArray, respons); */
          /*      } */
          /*      sprintf(http->resp->data, "%s\n", json_object_to_json_string(responsArray)); */
          /*      printf("%s\n", json_object_to_json_string(responsArray)); */
          /*      break; */
          /* default: */
          /*      goto err; */
          /*      return -1; */
          /* } */
          mysql_close(&mysql);
          return 0;
     }

err:
     mysql_close(&mysql);
     return -1;
}
