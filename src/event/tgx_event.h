#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"

/* 将特定平台的event转换成通用的event */

#define TGX_EVENT_IN		TGX_BITSET(0)
#define TGX_EVENT_OUT		TGX_BITSET(1)
#define TGX_EVENT_ERR		TGX_BITSET(2)
#define TGX_EVENT_HUP		TGX_BITSET(3)
#define TGX_EVENT_PRI		TGX_BITSET(4)
#define TGX_EVENT_RDHUP		TGX_BITSET(5)

#define TGX_EVENT_CTL_ADD 0
#define TGX_EVENT_CTL_MOD 1
#define TGX_EVENT_CTL_DEL 2


typedef int (*tgx_handler_t)(tgx_cycle_t *tcycle, void *context, int event);

struct tgx_schedule_s {
	int fd;
	tgx_handler_t handler;
	void *context;
};

struct tgx_event_s {
	tgx_cycle_t    *tcycle;
	tgx_schedule_t **sched_array;
	size_t maxfds;
	size_t usedfds;

#ifdef USE_LINUX_EPOLL
	int epoll_fd; // epoll多路系统用到的句柄, 这个是系统的核心
	struct epoll_event *epoll_events;   // 这个相当于内存， 每次poll以后数据就存放在这个区域
#endif
	void (*event_free)          (tgx_event_t *te);
	int  (*ctl)           (tgx_event_t *te, int op, int fd, int event);
	int  (*get_event)     (tgx_event_t *te, int index);
	int  (*get_fd)        (tgx_event_t *te, int index);
	int  (*get_next_index)(tgx_event_t *te, int index);
	int  (*poll)          (tgx_event_t *te, int timeout_ms); // 这个是CPU
};

tgx_event_t*    tgx_event_init (tgx_cycle_t *tcycle, int maxfds);
void		    tgx_event_free (tgx_event_t *te);

int				tgx_event_schedule_register(tgx_event_t *te, 
								 int fd, 
								 tgx_handler_t handler,
								 void* context);
int				tgx_event_schedule_unregister(tgx_event_t *te, int fd);

int				tgx_event_poll(tgx_event_t *te, int timeout_ms);


int				tgx_event_ctl(tgx_event_t *te, 
								  int op,
							      int fd, 
							  	  int event);
int				tgx_event_get_fd        (tgx_event_t *te, int index);
tgx_handler_t   tgx_event_get_handler   (tgx_event_t *te, int fd);
int             tgx_event_set_handler   (tgx_event_t *te, 
										 int fd, 
										 tgx_handler_t handler, 
										 void *context);

void*			tgx_event_get_context   (tgx_event_t *te, int fd);
int				tgx_event_get_event     (tgx_event_t *te, int index);
int				tgx_event_get_next_index(tgx_event_t *te, int index);

// reactor事件系统初始化(基于linux epoll)
int				tgx_linux_epoll_init(tgx_event_t *te);
