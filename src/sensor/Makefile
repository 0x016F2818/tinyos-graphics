CC = gcc

INCLUDE += -I/usr/include/mysql -I../db/db_connector/include/ \
		   -I../motes -I./ -I../../lib/sf

LIB = -L/usr/lib/i386-linux-gnu

SRC += ../../lib/sf/serialsource.c ../../lib/sf/sfsource.c sf.c ../db/db_connector/src/in.c \
	       ../db/db_connector/src/out.c ../motes/tgx_serial_msg.c \
	       ../db/db_connector/src/get_connect_handle.c ../../lib/sf/message.c pid_vector.c\

CFLAGS += -lm -lmysqlclient -lpthread -ldl -lz

STATIC = -static

all:
	$(CC) $(SRC) $(INCLUDE) $(LIB) $(CFLAGS) tgx_sensor_client.c -o tgx_sensor_client -lm
	$(CC) $(SRC) $(INCLUDE) $(LIB) $(CFLAGS) tgx_sensor_server.c -o tgx_sensor_server -lm
static:
	$(CC) $(SRC) $(INCLUDE) $(LIB) $(CFLAGS) tgx_sensor_client.c -o tgx_sensor_client_static $(STATIC) -lm
	$(CC) $(SRC) $(INCLUDE) $(LIB) $(CFLAGS) tgx_sensor_server.c -o tgx_sensor_server_static $(STATIC) -lm

clean:
	rm -rf *.[aioS]
	rm -rf sserver sclient
	rm -rf *\~
	rm -rf .*.swp
