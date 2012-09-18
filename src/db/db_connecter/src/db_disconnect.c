#include "../include/sensor.h"
void db_disconnect(db_connect_t *db_connector){

    mysql_close((MYSQL *)(db_connector->db_handler));
    free(db_connector->db_handler);
}

