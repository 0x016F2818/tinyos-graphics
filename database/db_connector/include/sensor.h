#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include"mysql/mysql.h"

#define DB_COMMAND_LENGTH           200
#define NETWORK_FLUSH_TIMEOUT       5

#define DB_QUERY                    -1001
#define DB_NO_QUERY                 -1002
#define DB_VALUE                    -1003
#define DB_NO_VALUE                 -1004

/*#define DB_SELEC_ALL_NODE_INFO      -2001*/
/*#define DB_SELEC_NETWORK_INFO       -2002*/
/*#define DB_SELEC_ALL_RECORD         -2003*/
/*#define DB_SELEC_TIMERANGE_RECORD   -2004*/
/*#define DB_SELEC_TIME_AHEAD_RECORD  -2005*/
/*#define DB_SELEC_TIME_BACK_RECORD   -2006*/

#define DB_QUERY_NODE_INFO(x) \
    x = calloc(1, sizeof(*x)); \
x->sensor_info = NULL; \
x->network_info = NULL;\
x->node_info = calloc(1, sizeof(*node_info)); \
x->node_info->x = DB_QUERY; \
x->node_info->y = DB_QUERY; \
x->node_info->z = DB_NO_QUERY

/*################################################*/
typedef struct sensor_msg {
    int         network_id;
    char        network_name[50];
	int         node_id;
    int         parent_id;
    char        work_state[50];
    char        node_status[50];
    int         power;
	struct {
		double  x;
		double  y;
	} position;
} node_t;

typedef struct {
    int         network_id;
    char        network_name[50];
    int         node_id;
	double      temp;
	double      photo;
	double      sound;
	double      x_acc;
	double      y_acc;
	double      x_mag;
	double      y_mag;
	char        time[50]; // ctime()返回的指针是一个指向静态区的指针, 因此这里没有分配空间
} sensor_t;

typedef struct {
    int         network_id;
    char        network_name[50];
    int         node_id;
    int         parent_id;
    int         quality;
} network_t;

/*typedef struct {*/
/*enum {*/
/*DB_SELEC_ALL_RECORD = 0,*/
/*DB_SELEC_NETWORK_INFO,*/
/*DB_SELEC_ALL_NODE_INFO,*/
/*DB_SELEC_TIME_BACK_RECORD,*/
/*DB_SELEC_TIMERANGE_RECORD,*/
/*DB_SELEC_TIME_AHEAD_RECORD*/
/*}search_type;*/
/*char        starttime[20];*/
/*char        endtime[20];*/
/*int         record_num;*/
/*}select_options_t;*/

/*typedef struct {*/
/*node_t      *node_info;*/
/*sensor_t    *sensor_info;*/
/*network_t   *network_info;*/
/*select_options_t *select_option;*/
/*}sensor_msg_t;*/

typedef struct {
    char        host[50];
    char        user[50];
    char        password[60];
    char        db_name[50];
}db_connect_info_t;

/*typedef struct {*/
/*enum {*/
/*DB_MYSQL = 0, */
/*DB_ORACLE, */
/*DB_SQL_SERVER*/
/*}db_type;*/
/*void        *db_handler;*/
/*}db_connect_t;*/
    
/*################################################*/
void fill_connect_info(db_connect_info_t *db_connect_info);
int get_db_handler(MYSQL *mysql);

int insert_sense_record(sensor_t info);
int update_network(network_t info);
int update_node_info(node_t info);

int get_all_node_num(int *node_num);
int get_absolute_record_num(sensor_t *info,char *net_name,int nod_id,char * start_time,char *end_time,long *record_num);

int get_all_node_info(node_t *info);
int get_network_info(network_t *info);
long get_all_record(sensor_t *info);
int get_latest_record(sensor_t *info,char *net_name,int nod_id,char *sense);
int get_absolute_record(sensor_t *info,char *net_name,int nod_id,char *sense,char *start_time,char *end_time);
long get_relative_record(sensor_t *info,char *net_name,int nod_id,char *sense,char *start_time,long record_num);
