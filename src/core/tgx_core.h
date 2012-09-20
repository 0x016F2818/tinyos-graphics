#pragma once

typedef struct tgx_task_s					tgx_task_t;
typedef struct tgx_task_schedule_s			tgx_task_schedule_t;
typedef struct tgx_tcycle_s					tgx_cycle_t;
typedef struct tgx_config_s					tgx_config_t;
typedef struct tgx_connection_s				tgx_connection_t;
typedef struct tgx_event_s					tgx_event_t;	
typedef struct tgx_schedule_s				tgx_schedule_t;
typedef struct tgx_http_parser_s			tgx_http_parser_t;
typedef struct tgx_module_http_s			tgx_module_http_t;

typedef struct {
	char e_404[256];

} tgx_err_page_t;

// tcycle是注入到所有的执行handler中的“全局变量”,
struct tgx_tcycle_s {
	int					listen_fd;
	int					port;
	int					maxfds;
	char				srv_root[1024];
	char				lock_file[1024];
	char				log_file[1024];
	tgx_err_page_t		err_page;
	tgx_event_t			*tevent;
	int					event_timeout_ms;

	tgx_task_schedule_t *task_sched;
	int					numThreads;
};

// 目前还没有更好的办法解决鸡生蛋的问题
// 所以所有的buffer都使用下面这个结构体
// 其他的还是使用char
#ifndef TGX_STRING_T
#define TGX_STRING_T
typedef struct {
	int size;
	char *data;
} tgx_string_t;
#endif

// 为了让用户只拿到tgx_module_http_t类型的参数， 
// 这里只能采用一个封装器了， 任务调度器拿到的是
// 是我包装的函数， 然后在这个函数内部去执行用户
// 的模块函数

#ifndef TGX_BITSET 
#define TGX_BITSET(x) (1 << x)
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
#include <dlfcn.h>

#include "tgx_log.h"
#include "tgx_connection.h"
#include "tgx_task.h"
#include "tgx_event.h"
#include "tgx_http_parser.h"
#include "tgx_module_internal.h"

// 使用了垃圾回收器libgc
#undef USE_GARBAGE_COLLECTION

#ifdef USE_GARBAGE_COLLECTION
#include <gc.h>
#define malloc(n) GC_malloc(n)
#define realloc(m, n) GC_realloc(m, n)
#if !defined(GC_calloc)
#define GC_calloc
//inline void *GC_calloc(size_t nmemb, size_t size)
//{
	//void *p = GC_malloc(nmemb * size);
	//if (!p) return NULL;
	//memset(p, 0, nmemb * size);
	//return p;
//}
#endif
//#define calloc(m, n) GC_calloc(m, n)
#define free
#else
#include <malloc.h>
#endif
