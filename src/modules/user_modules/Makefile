CC = gcc

#MODULE_SRC = nodeInfo.c
#MODULE_OBJ = nodeInfo.wsn

MODULE_SRC = realTime.c
MODULE_OBJ = realTime.wsn

#MODULE_SRC = abso_time_record.c
#MODULE_OBJ = abso_time_record.wsn

DB_CONNECTOR_DIR = ../../db/db_connector


INCLUDE += -I$(DB_CONNECTOR_DIR)/include -I/usr/include/mysql
LIB += 
CFLAGS += $(INCLUDE) $(LIB) -ljson -lmysqlclient -shared

BASE_SRC = $(DB_CONNECTOR_DIR)/src/in.c $(DB_CONNECTOR_DIR)/src/out.c \
		   $(DB_CONNECTOR_DIR)/src/get_connect_handle.c \

all:
	$(CC) $(BASE_SRC) $(MODULE_SRC) $(CFLAGS) -o $(MODULE_OBJ)
