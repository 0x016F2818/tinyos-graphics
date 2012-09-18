#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <json/json.h>

int main(int argc, char **argv)
{
  struct json_object *new_obj;
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
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  new_obj = json_object_object_get(new_obj, "1");
  printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));
  new_obj = json_object_object_get(new_obj, "liupeng");
  new_obj = json_object_object_get(new_obj, "age");
  int liupeng_age = json_object_get_int(new_obj);
  printf("liupeng age = %d\n", liupeng_age);
  /*json_object_put(new_obj);*/
  return 0;
}
