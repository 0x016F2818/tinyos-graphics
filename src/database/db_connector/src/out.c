#include "../include/sensor.h"

int get_all_node_info(MYSQL *mysql,node_t *info){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call sp_get_all_node_info()");
    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[node_get_info]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }

    long i = 0;
    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))){
            info[i].network_id   = atoi(record[0]);
            info[i].node_id      = atoi(record[2]);
            info[i].parent_id    = atoi(record[3]);
            info[i].power        = atoi(record[6]);
            info[i].position.x   = strtod(record[7],NULL);
            info[i].position.y   = strtod(record[8],NULL);
            strcpy(info[i].network_name,record[1]);
            strcpy(info[i].work_state,record[4]);
            strcpy(info[i].node_status,record[5]);
            i++;
        } 
        mysql_free_result(results);
    }
    else{
        if (mysql_field_count(mysql) == 0) {
            printf("%lld rows affected\n",
                    mysql_affected_rows(mysql));
        }
        else{
            printf("Could not retrieve result set\n");
            return -1;
        }
    }
    do {
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    

    return i;
}

//#################################################
int get_network_info(MYSQL *mysql,network_t *info){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call sp_get_network_info()");
    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[node_get_info]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }

    long i = 0;
    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))){
            info[i].network_id   = atoi(record[0]);
            info[i].node_id      = atoi(record[2]);
            info[i].parent_id    = atoi(record[3]);
            info[i].quality      = atoi(record[4]);
            strcpy(info[i].network_name,record[1]);
            i++;
        } 
        mysql_free_result(results);
    }
    else{
        if (mysql_field_count(mysql) == 0) {
            printf("%lld rows affected\n",
                    mysql_affected_rows(mysql));
        }
        else{
            printf("Could not retrieve result set\n");
            return -1;
        }
    }
    do {
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    

    return i;
}

//#################################################
long get_all_record(MYSQL *mysql,sensor_t *info){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call sp_get_all_record()");
    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[all_record]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }

    long i = 0;
    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))) {
            info[i].network_id  = atoi(record[0]);
            info[i].node_id     = atoi(record[2]);
            info[i].temp  = strtod(record[3],NULL);
            info[i].photo = strtod(record[4],NULL);
            info[i].sound = strtod(record[5],NULL);
            info[i].x_acc = strtod(record[6],NULL);
            info[i].y_acc = strtod(record[7],NULL);
            info[i].x_mag = strtod(record[8],NULL);
            info[i].y_mag = strtod(record[9],NULL);
            strcpy(info[i].network_name,record[1]);
            strcpy(info[i].time,record[14]);
            i++;
        }
        mysql_free_result(results);
    }
    else{
        if (mysql_field_count(mysql) == 0) {
            printf("%lld rows affected\n",
                    mysql_affected_rows(mysql));
        }
        else{ 
            printf("Could not retrieve result set\n");
            return -1;
        }
    }
    do {
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    

    return i;
}

//#################################################
//
//RETURNS:the line number of the records
//#################################################
int get_latest_record(MYSQL *mysql,sensor_t *info,char *net_name,int nod_id,char *sense){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call sp_get_latest_record('%s',%d,'%s')",net_name,nod_id,sense);
    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[all_record]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }

    int i = 0;
    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))) {
            info[i].temp  = strtod(record[0],NULL);
            strcpy(info[i].time,record[1]);
            i++;
        }
        mysql_free_result(results);
    }
    else{
        if (mysql_field_count(mysql) == 0) {
            printf("%lld rows affected\n",
                    mysql_affected_rows(mysql));
        }
        else{ 
            printf("Could not retrieve result set\n");
            return -1;
        }
    }
    do {
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    

    return i;
}

//#################################################
//
//RETURNS:the line number of the records
//#################################################
int get_absolute_record(MYSQL *mysql,sensor_t *info,char *net_name,int nod_id,char sense[50],char * start_time,char *end_time){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call sp_get_absolute_record('%s',%d,'%s','%s','%s')",net_name,nod_id,sense,start_time,end_time);
    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[all_record]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }

    int i = 0;
    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))) {
            info[i].temp = strtod(record[0],NULL);
            strcpy(info[i].time,record[1]);
            i++;
        }
        mysql_free_result(results);
    }
    else{
        if (mysql_field_count(mysql) == 0) {
            printf("%lld rows affected\n",
                    mysql_affected_rows(mysql));
        }
        else{ 
            printf("Could not retrieve result set\n");
            return -1;
        }
    }
    do {
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    

    return i;
}

//#################################################
//
//RETURNS:the line number of the records
//#################################################
long get_relative_record(MYSQL *mysql,sensor_t *info,char *net_name,int nod_id,char *sensor,char *start_time,long record_num){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call sp_get_relative_record('%s',%d,'%s',%s,%ld)",net_name,nod_id,sensor,start_time,record_num);

    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[all_record]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }

    long i = 0;
    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))) {
            info[i].temp = strtod(record[0],NULL);
            strcpy(info[i].time,record[1]);
            i++;
        }
        mysql_free_result(results);
    }
    else{
        if (mysql_field_count(mysql) == 0) {
            printf("%lld rows affected\n",
                    mysql_affected_rows(mysql));
        }
        else{ 
            printf("Could not retrieve result set\n");
            return -1;
        }
    }
    do {
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    

    return i;
}

//#################################################
int get_all_node_num(MYSQL *mysql,int *node_num){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"select count(*) from viw_node");
    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[node_get_num]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }

    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))) {
            *node_num = atoi(record[0]);
        }
        mysql_free_result(results);
    }
    else{
        if (mysql_field_count(mysql) == 0) {
            printf("%lld rows affected\n",
                    mysql_affected_rows(mysql));
        }
        else{ 
            printf("Could not retrieve result set\n");
            return -1;
        }
    }
    do {
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    

    return 0;
}

//#################################################
int get_absolute_record_num(MYSQL *mysql,sensor_t *info,char *net_name,int nod_id,char * start_time,char *end_time,long *record_num){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call sp_get_absolute_record_num('%s',%d,'%s','%s')",net_name,nod_id,start_time,end_time);

    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[node_get_num]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }

    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))) {
            *record_num = atoi(record[0]);
        }
        mysql_free_result(results);
    }
    do {
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    

    return 0;
}
