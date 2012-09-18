#include<stdio.h>
#include<stdlib.h>
#include"mysql/mysql.h"
#include<string.h>
#include"errno.h"

#define DB_COMMAND_LENGTH            200

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

/*sensor_msg_t *msg;*/
/*DB_QUERY_NODE_INFO(msg);*/

/*done() */
/*{*/
/*if (x == DB_QUERY) zhuzhuang()*/

/*z = DB_NO_VALUE;*/
/*}*/
/*################################################*/
typedef struct sensor_msg {
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
    int         node_id;
	double      temp;
	double      photo;
	double      sound;
	double      x_acc;
	double      y_acc;
	double      x_mag;
	double      y_mag;
	char        *time; // ctime()返回的指针是一个指向静态区的指针, 因此这里没有分配空间
}sensor_t;

typedef struct {
    int         node_id;
    int         parent_id;
    double      quality;
}network_t;

typedef struct {
    enum {
        DB_SELEC_ALL_RECORD = 0,
        DB_SELEC_NETWORK_INFO,
        DB_SELEC_ALL_NODE_INFO,
        DB_SELEC_TIME_BACK_RECORD,
        DB_SELEC_TIMERANGE_RECORD,
        DB_SELEC_TIME_AHEAD_RECORD
    }search_type;
    char        starttime[20];
    char        endtime[20];
    int         record_num;
}select_options_t;

typedef struct {
    node_t      *node_info;
    sensor_t    *sensor_info;
    network_t   *network_info;
    select_options_t *select_option;
}sensor_msg_t;

typedef struct {
    char        host[40];
    char        user[20];
    char        password[60];
    char        db_name[20];
}db_connect_info_t;

typedef struct {
    enum {
        DB_MYSQL = 0, 
        DB_ORACLE, 
        DB_SQL_SERVER
    }db_type;
    void        *db_handler;
}db_connect_t;
    
/*################################################*/
int get_db_handler(MYSQL *mysql,db_connect_info_t db_connect_info);

int insert_sense_record(MYSQL *mysql,sensor_t info);
int update_network(MYSQL *mysql,network_t info);
int update_node_info(MYSQL *mysql,node_t info);

int all_node_info(MYSQL *mysql,node_t *info);
int all_record(MYSQL *mysql,sensor_t *info);

int db_connect(db_connect_t *db_connector,db_connect_info_t db_connect_info);
void db_disconnect(db_connect_t *db_connector);
int db_push(db_connect_t db_connector,sensor_msg_t sensor_msg);
int db_pull(db_connect_t *db_connector,sensor_msg_t *sensor_msg);
