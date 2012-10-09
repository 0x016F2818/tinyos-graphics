#include "../include/sensor.h"

int main(int argc,char *argv[])
{
    int ret;
    MYSQL mysql;
    char command[DB_COMMAND_LENGTH] = "\0";
    db_connect_info_t db_connect_info;

    strcpy(db_connect_info.host,"127.0.0.1");
    strcpy(db_connect_info.user,"root");
    strcpy(db_connect_info.password,"arnold");
    strcpy(db_connect_info.db_name,"glacier_db");

    if(get_db_handler(&mysql,db_connect_info) == -1){
        return -1;
    }
   
    sprintf(command,"source table_creater.sql");
    ret = mysql_real_query(&mysql,command,(unsigned)strlen(command));

    if (ret){
        printf("Error exec command: %s\n",mysql_error(&mysql));
        return -1;
    }
    else{
        printf("[node_get_info]: %ld products updated successfully!\n",(long) mysql_affected_rows(&mysql));
    }

    mysql_close(&mysql);
    return 0;
}
