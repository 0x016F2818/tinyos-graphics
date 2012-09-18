#include "../include/sensor.h"

int all_node_info(MYSQL *mysql,node_t *info){
    int ret;
    char command[DB_COMMAND_LENGTH] = "\0";
    MYSQL_ROW record;
    MYSQL_RES *results;

    sprintf(command,"call all_node_info_sp()");
    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[all_node_info]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }


    if (ret) {
         printf("Could not execute statement(s)");
         mysql_close(mysql);
         exit(0);
    }

    /* process each statement result */
    int i = 0;
    /* did current statement return data? */
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
    else{      /* no result set or error */
        if (mysql_field_count(mysql) == 0) {
            printf("%lld rows affected\n",
            mysql_affected_rows(mysql));
        }
        else{  /* some error occurred */
            printf("Could not retrieve result set\n");
            return -1;
        }
    }
    do {
    /* more results? -1 = no, >0 = error, 0 = yes (keep looping) */
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    


    mysql_free_result(results);
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

    sprintf(command,"call all_record_sp()");
    ret = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (ret){
        printf("Error exec command: %s\n",mysql_error(mysql));
        return -1;
    }
    else{
        printf("[all_record]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }

    int i = 0;
    /* did current statement return data? */
    results = mysql_store_result(mysql);
    if (results) {
    while((record = mysql_fetch_row(results))) {
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
    else{      /* no result set or error */
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
    /* more results? -1 = no, >0 = error, 0 = yes (keep looping) */
        if ((ret = mysql_next_result(mysql)) > 0)
            printf("Could not execute statement\n");
    } while (ret == 0);    

    return i;
}
