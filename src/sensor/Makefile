CC = gcc

INCLUDE += -I/usr/include/mysql -I../db/include/ \
		   -I../motes/ -I./ -I../../lib/sf/

LIB = -L/usr/lib/i386-linux-gnu/

SRC += ../../lib/sf/serialsource.c ../../lib/sf/sfsource.c sf.c ../db/src/in.c \
	       ../db/src/out.c ../motes/tgx_serial_msg.c \
	       ../db/src/get_connect_handle.c ../../lib/sf/message.c \

CFLAGS += -lmysqlclient

all:
	$(CC) $(SRC) $(INCLUDE) $(LIB) $(CFLAGS) tgx_sensor_client.c -o tgx_sensor_client
	$(CC) $(SRC) $(INCLUDE) $(LIB) $(CFLAGS) tgx_sensor_server.c -o tgx_sensor_server

clean:
	rm -rf *.[aioS]
	rm -rf sserver sclient
	rm -rf *\~
	rm -rf .*.swp