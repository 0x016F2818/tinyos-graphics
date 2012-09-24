#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"

typedef struct tgx_task_queue_s				tgx_task_queue_t;

// *******************************************************
// PUBLIC
// ******************************************************
// 用户需要自己填写下面的字段， 不需要的字段填写NULL即可
// 有两个回调函数， 一个在post到queue之前执行, 这样可以在
// 调度器内部检查此次task是否合法， 还有一个回调函数在task
// 执行完产生， 这样用户就可以继续POST构造链条， 或者执行
// 别的


struct tgx_task_s {
	pthread_mutex_t     *task_lock;
	void				*context; // 这个将作为函数的形参

// 函数指针变量
	int (*on_post)         (void *context);
	int (*on_task_complete)(void *context, int err);
	int (*task_handler)    (void *context);
};

// 这个是任务调度器
struct tgx_task_schedule_s {
	pthread_mutex_t		*sched_lock;
	tgx_task_queue_t	*task_queue;
	pthread_t			*tids;
	int					n_threads;
	int					keep_alive:1;
};

// 用户使用的接口只有4个
tgx_task_schedule_t* tgx_task_schedule_init(int n_threads);
void tgx_task_schedule_destroy(tgx_task_schedule_t *t_task_sched);
tgx_task_t *tgx_task_init(void);
int tgx_post_task(tgx_task_schedule_t *t_task_sched, tgx_task_t *new_task);

// **********************************************
// Private, 用于实现任务排队
// **********************************************

// 队列本身没有锁的机制， 无法保证数据一致性， 锁的任务交给上层的
// 调度器， 信号量很形象的表示了队列的元素个数， 当需要从队列中取出
// 元素时， 调度器的lock保证了只有一个线程修改队列， 另外信号量很优雅的
// 让信号睡眠
typedef struct __tgx_task_internal_s		__tgx_task_internal_t;
struct __tgx_task_internal_s {
	tgx_task_t						*task;
	struct __tgx_task_internal_s	*next;
	struct __tgx_task_internal_s	*prev;
};

struct tgx_task_queue_s {
	__tgx_task_internal_t   *head;
	__tgx_task_internal_t   *tail;

	sem_t				    *resource;
	int						n_task;
};

tgx_task_queue_t* tgx_task_queue_init(void);
void tgx_task_queue_empty(tgx_task_queue_t *t_task_queue);
int tgx_task_queue_push_back(tgx_task_queue_t *t_task_queue, __tgx_task_internal_t *t_task_internal);
tgx_task_t* tgx_task_queue_pop_front(tgx_task_queue_t *t_task_queue);
int tgx_task_queue_signal(tgx_task_queue_t *t_task_queue);
int tgx_task_queue_wait(tgx_task_queue_t *t_task_queue);
