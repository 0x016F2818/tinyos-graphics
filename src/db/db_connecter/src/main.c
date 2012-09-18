#include "../include/sensor.h"

int main(int argc,char *argv[])
{
    db_connect_t db_connector;
    db_connect_info_t db_connect_info;

    sensor_msg_t inser_info;
    sensor_msg_t *get_info;

    inser_info.node_info = (node_t *)malloc(sizeof(node_t));
    inser_info.sensor_info = (sensor_t *)malloc(sizeof(sensor_t));
    inser_info.network_info = (network_t *)malloc(sizeof(network_t));
    
    get_info = (sensor_msg_t *)malloc(sizeof(sensor_msg_t) * 10);

    db_connector.db_type = 0;
    strcpy(db_connect_info.host,"10.18.46.111");
    strcpy(db_connect_info.user,"tinyos");
    strcpy(db_connect_info.password,"tinyos");
    strcpy(db_connect_info.db_name,"tinyos");
    
    if(db_connect(&db_connector,db_connect_info) == -1){
        return -1;
    }

    inser_info.node_info->node_id            = 3;
    inser_info.node_info->parent_id          = 2;
    inser_info.node_info->power             = 70;
    inser_info.node_info->position.x        = 20;
    inser_info.node_info->position.y        = 30;
    strcpy(inser_info.node_info->work_state,"work");
    strcpy(inser_info.node_info->node_status,"normal");

    inser_info.sensor_info->temp            = 26;
    inser_info.sensor_info->photo           = 98;
    inser_info.sensor_info->sound           = 21;
    inser_info.sensor_info->x_acc           = 34;
    inser_info.sensor_info->y_acc           = 0;
    inser_info.sensor_info->x_mag           = 22;
    inser_info.sensor_info->y_mag           = 33;

    ///////////////////////////////////////////////
    db_push(db_connector,inser_info); 
    {
    //db_pull(*&db_connector,get_info);
    //int i,j;
    ////j = all_record(&mysql,info);
    ////j = all_node_info(&mysql,info);
    //if(j==-1)
        //return -1;
    //else{
        //for(i = 0;i < j;i++){
            //printf("node_id:%d\t"          ,get_info[i].node_info->node_id);
            //printf("parent_id:%d\t"        ,get_info[i].node_info->parent_id);
            //printf("power:%d\t"            ,get_info[i].node_info->power);
            //printf("position_x:%6.2lf\t"   ,get_info[i].node_info->position.x);
            //printf("position_y:%6.2lf\t"   ,get_info[i].node_info->position.y);
            //printf("temp:%6.2lf\t"         ,get_info[i].sensor_info->sensor.temp);
            //printf("photo:%6.2lf\t"        ,get_info[i].sensor_info->sensor.photo);
            //printf("sound:%6.2lf\t"        ,get_info[i].sensor_info->sensor.sound);
            //printf("x_acc:%6.2lf\t"        ,get_info[i].sensor_info->sensor.x_acc);
            //printf("y_acc:%6.2lf\t"        ,get_info[i].sensor_info->sensor.y_acc);
            //printf("x_mag:%6.2lf\t"        ,get_info[i].sensor_info->sensor.x_mag);
            //printf("y_mag:%6.2lf\t"        ,get_info[i].sensor_info->sensor.y_mag);
            //printf("\n");
        //}
    //}
    }
    db_disconnect(&db_connector);
    return 0;
}
