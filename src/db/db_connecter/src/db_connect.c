#include "../include/sensor.h"

int db_connect(db_connect_t *db_connector,db_connect_info_t db_connect_info){
        if(!(db_connector->db_handler = (MYSQL *)malloc(sizeof(MYSQL)))){
            printf("db_handler:malloc failed!\n");
            return -1;
        }

        get_db_handler((MYSQL *)(db_connector->db_handler),db_connect_info);
        return 0;
}
