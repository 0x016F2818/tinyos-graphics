#include "../include/sensor.h"

int all_node_info(MYSQL *mysql,node_t *info){
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

    int i = 0;
    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))){
            info[i].node_id      = atoi(record[0]);
            info[i].parent_id    = atoi(record[1]);
            info[i].power       = atoi(record[4]);
            info[i].position.x  = strtod(record[5],NULL);
            info[i].position.y  = strtod(record[6],NULL);
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
int all_record(MYSQL *mysql,sensor_t *info){
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

    int i = 0;
    results = mysql_store_result(mysql);
    if (results) {
        while((record = mysql_fetch_row(results))) {
            info[i].node_id = atoi(record[0]);
            info[i].temp  = strtod(record[1],NULL);
            info[i].photo = strtod(record[2],NULL);
            info[i].sound = strtod(record[3],NULL);
            info[i].x_acc = strtod(record[4],NULL);
            info[i].y_acc = strtod(record[5],NULL);
            info[i].x_mag = strtod(record[6],NULL);
            info[i].y_mag = strtod(record[7],NULL);
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
int get_latest_record(MYSQL *mysql,sensor_t *info,int id){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call sp_get_latest_record(%d,'temperature')",id);
    //ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    ret = mysql_query(mysql,command);
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
            //info[i].node_id = atoi(record[0]);
            info[i].temp  = strtod(record[0],NULL);
            //info[i].photo = strtod(record[2],NULL);
            //info[i].sound = strtod(record[3],NULL);
            //info[i].x_acc = strtod(record[4],NULL);
            //info[i].y_acc = strtod(record[5],NULL);
            //info[i].x_mag = strtod(record[6],NULL);
            //info[i].y_mag = strtod(record[7],NULL);
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
int get_absolute_record(MYSQL *mysql,sensor_t *info,int id,char * start_time,char *end_time){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call sp_get_absolute_record(%d,'temperature',%s,%s)",id,start_time,end_time);
    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    //ret = mysql_query(mysql,command);
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
            //info[i].node_id = atoi(record[0]);
            info[i].temp  = strtod(record[0],NULL);
            //info[i].photo = strtod(record[2],NULL);
            //info[i].sound = strtod(record[3],NULL);
            //info[i].x_acc = strtod(record[4],NULL);
            //info[i].y_acc = strtod(record[5],NULL);
            //info[i].x_mag = strtod(record[6],NULL); //info[i].y_mag = strtod(record[7],NULL);
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
