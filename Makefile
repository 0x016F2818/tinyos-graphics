all:
	gcc src/core/tgx.c src/core/tgx_connection.c \
		src/core/tgx_log.c  \
		src/core/tgx_task.c src/event/tgx_event.c \
		src/event/tgx_linux_epoll.c src/http/tgx_http.c \
		src/http/tgx_http_fsm.c src/motes/tgx_serial_msg.c \
		src/modules/tgx_module_internal.c \
		./lib/http-parser/http_parser.c \
		-L./lib/gc-7.2 -L./lib/http-parser -L./lib/json-c \
		-L/usr/local/lib -L/usr/lib \
		-L./lib/sf \
		-I./lib/gc-7.2 -I./lib/http-parser \
		-I../lib/json-c -I./lib/sf \
		-Isrc/core -Isrc/event -Isrc/http -Isrc/motes -Isrc/serial \
		-Isrc/modules \
		-lpthread -lmote -ljson	-lgc -ldl\
		-o bin/tinyos-graphics
	
clean:
	rm -rf bin/tinyos_graphics
