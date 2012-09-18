#include "../include/sensor.h"

//#################################################
int update_node_info(MYSQL *mysql,node_t info){
    int result;
    char command[DB_COMMAND_LENGTH] = "\0";

    if(info.node_id == DB_NO_VALUE){
        printf("Incomplete info!\n");
        return -1;
    }

    sprintf(command,"call update_node_info_sp(%d,'work','normal',%d,point(%f,%f))",info.node_id,info.power,info.position.x,info.position.y);

    result = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (result){
         printf("Error exec command: %s\n",mysql_error(mysql));
         return -1;
    }
    else{
        printf("[node_info_update]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }
    return 0;
}

//#################################################
//still waiting for being perfected and improved!
int update_network(MYSQL *mysql,network_t info){
    int result;
    char command[DB_COMMAND_LENGTH] = "\0";

    if(info.node_id == DB_NO_VALUE || info.parent_id == DB_NO_VALUE){
        printf("Incomplete info!\n");
        return -1;
    }

    sprintf(command,"call update_network_sp(%d,%d)",info.node_id,info.parent_id);

    result = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (result){
         printf("Error exec command: %s\n",mysql_error(mysql));
         return -1;
    }
    else{
        printf("[network_update]: %ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }
    return 0;
}

//#################################################
int insert_sense_record(MYSQL *mysql,sensor_t info){
    int result;
    char command[DB_COMMAND_LENGTH] = "\0";

    if(info.node_id == DB_NO_VALUE){
        printf("Incomplete info!\n");
        return -1;
    }

    sprintf(command,"call insert_sense_record_sp(%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,NULL,NULL,NULL)",info.node_id,info.temp,info.photo,info.sound,info.x_acc,info.y_acc,info.x_mag,info.y_mag);

    result = mysql_real_query(mysql,command,(unsigned int)strlen(command));
    if (result){
         printf("Error exec command: %s\n",mysql_error(mysql));
         return -1;
    }
    else{
        printf("%ld products updated successfully!\n",(long) mysql_affected_rows(mysql));
    }
    return 0;
}

//#################################################
