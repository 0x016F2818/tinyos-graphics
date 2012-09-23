#include "../include/sensor.h"

int main(int argc,char *argv[])
{
    MYSQL mysql;
    db_connect_info_t db_connect_info;

    node_t      node_inser_info;
    sensor_t    sensor_inser_info;
    network_t   network_inser_info;
    memset(&node_inser_info,0,sizeof(node_t));
    memset(&sensor_inser_info,0,sizeof(sensor_t));
    memset(&network_inser_info,0,sizeof(network_t));
    
    node_t      node_info[10000];
    sensor_t    sensor_info[10000];
    network_t   network_info[10000];
    memset(&node_info,0,sizeof(node_info));
    memset(&sensor_info,0,sizeof(sensor_info));
    memset(&network_info,0,sizeof(network_info));

        //strcpy(db_connect_info.host,"127.0.0.1");
        //strcpy(db_connect_info.user,"root");
        //strcpy(db_connect_info.password,"arnold");
        //strcpy(db_connect_info.db_name,"test4");

    //strcpy(db_connect_info.host,"10.18.38.14");
    strcpy(db_connect_info.host,"10.18.46.163");
    strcpy(db_connect_info.user,"tinyos");
    strcpy(db_connect_info.password,"njjizyj0826");
    strcpy(db_connect_info.db_name,"test4");

    //strcpy(db_connect_info.host,"10.18.46.111");
    //strcpy(db_connect_info.user,"tinyos");
    //strcpy(db_connect_info.password,"tinyos");
    //strcpy(db_connect_info.db_name,"icecream");
    if(get_db_handler(&mysql,db_connect_info) == -1){
        return -1;
    }
    int num = 0;
    
    //node_inser_info.node_id           = num; 
    //node_inser_info.power             = num;
    //node_inser_info.position.x        = 50 + num % 10;
    //node_inser_info.position.y        = 40 + num % 10;
    //strcpy(node_inser_info.network_name,"10.16.17.0");
    //strcpy(node_inser_info.work_state,"work");
    //strcpy(node_inser_info.node_status,"normal");

    //update_node_info(&mysql,node_inser_info);

    //network_inser_info.node_id      = num;
    //network_inser_info.parent_id    = num;
    //strcpy(network_inser_info.network_name,"10.16.17.0");

    //update_network(&mysql,network_inser_info);
    //for(num = 1;num <= 100;num++){
        //node_inser_info.node_id           = num; 
        //node_inser_info.power             = 90 + num % 10;
        //node_inser_info.position.x        = 50 + num % 10;
        //node_inser_info.position.y        = 40 + num % 10;
        //strcpy(node_inser_info.network_name,"10.16.17.0");
        //strcpy(node_inser_info.work_state,"work");
        //strcpy(node_inser_info.node_status,"normal");

        //update_node_info(&mysql,node_inser_info);

        //network_inser_info.node_id      = num;
        //network_inser_info.parent_id    = num - 1;
        //strcpy(network_inser_info.network_name,"10.16.17.0");

        //update_network(&mysql,network_inser_info);

        //strcpy(sensor_inser_info.network_name,"10.16.17.0");
        //sensor_inser_info.node_id         = num % 300;
        //sensor_inser_info.temp            = 26 + num % 10;
        //sensor_inser_info.photo           = 98 + num % 10;
        //sensor_inser_info.sound           = 21 + num % 10;
        //sensor_inser_info.x_acc           = 34 + num % 10;
        //sensor_inser_info.y_acc           = 0  + num % 10;
        //sensor_inser_info.x_mag           = 22 + num % 10;
        //sensor_inser_info.y_mag           = 33 + num % 10;

        //insert_sense_record(&mysql,sensor_inser_info);
    //}
    long i,j;
    
    //j = get_all_record(&mysql,sensor_info);
    j = get_network_info(&mysql,network_info);
    //j = get_all_node_info(&mysql,node_info);
    //j = get_latest_record(&mysql,sensor_info,"10.16.17.0",50,"temperature");
    //j = get_absolute_record(&mysql,sensor_info,"10.16.17.0",0,"temperature","2012-09-23 11:21:41","2012-09-23 11:21:42");
    //j = get_relative_record(&mysql,sensor_info,"10.16.17.0",89,"microphone","20120919171826",-2);
    if(j==-1)
        return -1;
    else{
        for(i = 0;i < j;i++){
            //printf("node_id:%d\t"          ,node_info[i].node_id);
            //printf("parent_id:%d\t"        ,node_info[i].parent_id);
            //printf("power:%d\t"            ,node_info[i].power);
            //printf("position_x:%6.2lf\t"   ,node_info[i].position.x);
            //printf("position_y:%6.2lf\t"   ,node_info[i].position.y);

            printf("network_id:%d \t"        ,network_info[i].network_id);
            printf("network_name:%s \t"      ,network_info[i].network_name);
            printf("node_id:%d \t"           ,network_info[i].node_id);
            printf("parent_id:%d \t"         ,network_info[i].parent_id);
            printf("quality:%d \t"           ,network_info[i].quality);

            //printf("network_id:%d\t"       ,sensor_info[i].network_id);
            //printf("network_name:%s\t"     ,sensor_info[i].network_name);
            //printf("node_id:%d\t"          ,sensor_info[i].node_id);
            //printf("temp:%6.2lf\t"         ,sensor_info[i].temp);
            //printf("photo:%6.2lf\t"        ,sensor_info[i].photo);
            //printf("sound:%6.2lf\t"        ,sensor_info[i].sound);
            //printf("x_acc:%6.2lf\t"        ,sensor_info[i].x_acc);
            //printf("y_acc:%6.2lf\t"        ,sensor_info[i].y_acc);
            //printf("x_mag:%6.2lf\t"        ,sensor_info[i].x_mag);
            //printf("y_mag:%6.2lf\t"        ,sensor_info[i].y_mag);
            //printf("insert_time:%s\t"      ,sensor_info[i].time);
            printf("\n");
        }
    }
    mysql_close(&mysql);
    return 0;
}
