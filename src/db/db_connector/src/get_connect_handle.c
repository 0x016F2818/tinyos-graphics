#include "../include/sensor.h"

void fill_connect_info(db_connect_info_t *db_connect_info){
    strcpy(db_connect_info->host,"10.18.46.169");
    strcpy(db_connect_info->user,"tinyos");
    strcpy(db_connect_info->password,"njjizyj0826");
    strcpy(db_connect_info->db_name,"tinyos");
}

int get_db_handler(MYSQL *mysql){

    db_connect_info_t db_connect_info;
    fill_connect_info(&db_connect_info);

    if(!mysql_init(mysql))
    {
        printf("mysql_init failed!\n");
        return -1;
    }
    if(!mysql_real_connect(mysql,db_connect_info.host,db_connect_info.user,db_connect_info.password,db_connect_info.db_name,0,NULL,CLIENT_MULTI_STATEMENTS))
    {
         printf("mysql_real_connect() failed!\n");
         mysql_close(mysql);
         return -1;
    }
    else 
        printf("connecting to mysql success...!!!\n");
    return 0;
}

