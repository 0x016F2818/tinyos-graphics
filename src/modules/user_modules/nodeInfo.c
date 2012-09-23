#include <stdio.h>
#include "tgx_module.h"
#include <mysql.h>
#include <stdlib.h>
#include <json/json.h>
#include "sensor.h"
#include <string.h>

int get_index(int *array, int len, int value, int *isNew)
{
	int k;
	for (k = 0; array[k] != 0 && k < len; k++) {
		if (array[k] == value) {
			*isNew = 0;
			return k;
		}
	}

	return (k >= len)? -1 : (*isNew = 1, array[k] = value, k);
}

int TGX_MODULE_HTTP_HANDLER(tgx_module_http_t *http)
{
/*int main(int argc, char *argv[]) */
/*{*/
	struct json_object *new_obj;
	http->resp->data = calloc(65536, sizeof(char));
	http->resp->size = 65536;
	MYSQL mysql;
	db_connect_info_t db_connect_info;

	node_t      node_inser_info[10000];
	memset(node_inser_info, 0, sizeof(node_inser_info));
	{
		strcpy(db_connect_info.host,"10.18.46.111");
		strcpy(db_connect_info.user,"tinyos");
		strcpy(db_connect_info.password,"tinyos");
		strcpy(db_connect_info.db_name,"tinyos");

		if(get_db_handler(&mysql,db_connect_info) == -1){
			return -1;
		}
	}

	int i;
	int nNode;
	nNode = get_all_node_info(&mysql, node_inser_info);
	printf("nNode = %d\n", nNode);
	if (nNode < 0) return -1;

	json_object *network = json_object_new_array(),
				*network_object[nNode],
				*node_array[nNode],
				*node;

	memset(network_object, 0, sizeof(network_object));
	memset(node_array,     0, sizeof(node_array));
	printf("sizeof(*network_object), sizeof(*node_array) = %d, %d\n", 
			sizeof(network_object),
			sizeof(node_array));


	/*for (i = 0; i < nNode; i++) {*/
		/*network_object = json_object_new_object();*/
		/*node_array     = json_object_new_array();*/
	/*}*/

	int flag[nNode];
	memset(flag, 0, sizeof(flag));

	int network_id, index, isNew, network_num = 0;
	for (i = 0; i < nNode; i++) {
		node = json_object_new_object();
		/*printf("writing to network json format...,%d\n", i);*/
		network_id = node_inser_info[i].network_id;
		/*printf("network_id = %d, node_id = %d\n", node_inser_info[i].network_id, node_inser_info[i].node_id);*/
		index = get_index(flag, nNode, network_id, &isNew);
		if (index < 0) 
			return -1;
		if (isNew) {
			network_num++;
			network_object[index] = json_object_new_object();
			node_array[index] = json_object_new_array();
			json_object_object_add(network_object[index], "network_id", 
					json_object_new_int(node_inser_info[i].network_id));
			json_object_object_add(network_object[index], "network_name", 
					json_object_new_string(node_inser_info[i].network_name));
			/*printf("new node: %d, %s\n", node_inser_info[i].network_id, node_inser_info[i].network_name);*/
		}
		json_object_object_add(node, "node_id", json_object_new_int(node_inser_info[i].node_id));
		json_object_object_add(node, "parent_id", json_object_new_int(node_inser_info[i].parent_id));
		json_object_object_add(node, "power", json_object_new_int(100));
		json_object_object_add(node, "position_x", json_object_new_double(node_inser_info[i].position.x));
		json_object_object_add(node, "position_y", json_object_new_double(node_inser_info[i].position.y));
		json_object_array_add(node_array[index], node);
	}

	// 将分派的结果进行组装
	/*int tmp;*/

	// 先排序， 顺便插入
	/*int j, k;
	for (i = 0; i < len - 1; i++) {
		for (j = i + 1; i < len; j++) {
			if (flag[i] > flag[j]) {
				tmp = flag[i];
				flag[i] = flag[j];
				flag[j] = tmp;
			}
		}
	}*/
	/*for (i = 0; i < network_num; i++)*/
		/*printf("%d ", flag[i]);*/
	/*printf("\n");*/
	/*printf("network_num = %d\n", network_num);*/
	for (i = 0; i < network_num; i++) {
		json_object_object_add(network_object[i], "nodes", node_array[i]);
		json_object_array_add (network, network_object[i]);
	}
	sprintf(http->resp->data, "%s\n", json_object_to_json_string(network));
	printf("%s\n", json_object_to_json_string(network));

	return 0;
}
