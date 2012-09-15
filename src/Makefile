all:
	gcc ./core/tgx.c ./core/tgx_connection.c \
		./core/tgx_cycle.c ./core/tgx_log.c  \
		./core/tgx_task.c ./event/tgx_event.c \
		./event/tgx_linux_epoll.c ./http/tgx_http.c \
		./http/tgx_http_fsm.c ./motes/tgx_serial_msg.c \
		../lib/http-parser/http_parser.c \
		-L../lib/gc-7.2 -L../lib/http-parser -L../json-c \
		-L/usr/local/lib -L/usr/lib \
		-L../lib/sf \
		-I../lib/gc-7.2 -I../lib/http-parser \
		-I../lib/json-c -I../lib/sf \
		-I./core -I./event -I./http -I./motes -I./serial \
		-lpthread -lmote -ljson	-lgc					\
		-o ./core/tinyos_graphics 
	
clean:
	rm -rf ./core/tinyos_graphics
