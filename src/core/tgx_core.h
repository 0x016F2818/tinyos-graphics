#pragma once

typedef struct tgx_task_s					tgx_task_t;
typedef struct tgx_task_schedule_s			tgx_task_schedule_t;
typedef struct tgx_tcycle_s					tgx_cycle_t;
typedef struct tgx_config_s					tgx_config_t;
typedef struct tgx_connection_s				tgx_connection_t;
typedef struct tgx_event_s					tgx_event_t;	
typedef struct tgx_schedule_s				tgx_schedule_t;

typedef struct tgx_http_parser_s			tgx_http_parser_t;


#undef USE_GARBAGE_COLLECTION
#ifdef USE_GARBAGE_COLLECTION
#include <gc.h>
#define malloc GC_malloc
#define realloc GC_realloc
#define calloc GC_calloc
#define free
#else
#include <malloc.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h> // TCPCORK
#include <pthread.h>
#include <json/json.h>
#include <semaphore.h>
#include <sys/resource.h>
#include <stdarg.h>
#include <getopt.h>

#include "tgx_string.h"
#include "tgx_cycle.h"
#include "tgx_log.h"
#include "tgx_connection.h"
#include "tgx_task.h"
#include "tgx_config.h"
#include "tgx_event.h"
#include "tgx_http_fsm.h"
#include "tgx_http.h"
