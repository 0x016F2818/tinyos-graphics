#include "tgx_task.h"

// task队列使用信号量来表示资源的多少， 主要封装在两个接口:
// tgx_task_queue_signal()
// tgx_task_queue_wait()

/*pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;

#define log_err printf

#define DEBUG(...) \
	do { \
		pthread_mutex_lock(&mylock); \
		fprintf(stderr, "[DEBUG]:%s:%d:%s(): ", __FILE__, __LINE__, __FUNCTION__); \
		fprintf(stderr, __VA_ARGS__); \
		pthread_mutex_unlock(&mylock); \
	} while(0)*/

static void *do_pthread(void *arg)
{
	int ret;
	tgx_task_schedule_t *t_task_sched = (tgx_task_schedule_t *)arg;
	tgx_task_t *task;

	while (t_task_sched->keep_alive) {
		ret = tgx_task_queue_wait(t_task_sched->task_queue);
		if (ret < 0) {
			log_err("tgx_task_queue_wait() error\n");
			pthread_exit((void *)0);
		}
		if (t_task_sched->keep_alive) {
			pthread_mutex_lock(t_task_sched->sched_lock);
			task = tgx_task_queue_pop_front(t_task_sched->task_queue);
			pthread_mutex_unlock(t_task_sched->sched_lock); 

			ret = task->task_handler(task->context);

			if (task->on_task_complete) {
				task->on_task_complete(task->context, ret);
			}
			free(task);

		} else {
			pthread_exit((void *)0);
		}
	}
	pthread_exit((void *)0);
}

tgx_task_schedule_t* tgx_task_schedule_init(int n_threads)
{
	int ret;

	if (n_threads < 1) {
		log_err("thread number now set to one\n");
		n_threads = 1;
	}

	// 1. new tgx_task_schedule_t
	tgx_task_schedule_t *t_task_sched;
	t_task_sched = calloc(1, sizeof(tgx_task_schedule_t));
	if (!t_task_sched) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}

	// 2. init mutex
	t_task_sched->sched_lock = calloc(1, sizeof(pthread_mutex_t));
	if (!t_task_sched->sched_lock) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}
	ret = pthread_mutex_init(t_task_sched->sched_lock, NULL);
	if (ret < 0) {
		log_err("pthread_mutex_init():%s\n", strerror(errno));
		return NULL;
	}

	// 3. init task queue
	t_task_sched->task_queue = tgx_task_queue_init();
	if (!t_task_sched->task_queue) {
		log_err("tgx_task_queue_init() error\n");
		return NULL;
	}

	// 4. init tids
	t_task_sched->tids = calloc(n_threads, sizeof(pthread_t));
	if (!t_task_sched->tids) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}
	
	// 5. init n_threads
	t_task_sched->n_threads = n_threads;

	/*pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);*/

	// 6. start threads
	int i;
	for (i = 0; i < n_threads; i++) {
		ret = pthread_create(&t_task_sched->tids[i], NULL, do_pthread, (void *)t_task_sched);
		if (ret < 0) {
			log_err("pthread_create():%s\n", strerror(errno));
			return NULL;
		}
	}
	/*printf("before pthread_getconcurrency() = %d\n", pthread_getconcurrency());*/

	/*pthread_setconcurrency(n_threads);*/

	/*printf("after pthread_getconcurrency() = %d\n", pthread_getconcurrency());*/
	/*pthread_attr_destroy(&attr);*/
	
	// 7. set keep alive
	t_task_sched->keep_alive = 1;

	// 8. everything is fine, then return struct
	return t_task_sched;
}

void tgx_task_schedule_destroy(tgx_task_schedule_t *t_task_sched)
{
	int ret, i;

	if (!t_task_sched) {
		log_err("null pointer\n");
		return;
	}

	// 1. destroy all working thread
	t_task_sched->keep_alive = 0;

	// 假装增加n_threads个task给队列， 这样可以唤醒n_threads个线程
	for (i = 0; i < t_task_sched->n_threads; i++) {
		ret = tgx_task_queue_signal(t_task_sched->task_queue);
		if (ret < 0) {
			log_err("tgx_task_queue_broadcast() error\n");
			return;
		}
	}

	// 对每一个线程pthread_join， 回收线程资源
	for (i = 0; i < t_task_sched->n_threads; i++) {
		/*DEBUG("pthread join......\n");*/
		ret = pthread_join(t_task_sched->tids[i], NULL);
		if (ret < 0) {
			log_err("pthread_join():%s\n", strerror(errno));
			return;
		}
	}

	// 2. destroy sched_lock
	if (t_task_sched->sched_lock) {
		ret = pthread_mutex_destroy(t_task_sched->sched_lock);
		if (ret < 0) {
			log_err("pthread_mutex_destroy():%s\n", strerror(errno));
			return;
		}
		free(t_task_sched->sched_lock);
	}

	// 3. destroy queue
	if (t_task_sched->task_queue) {
		tgx_task_queue_empty(t_task_sched->task_queue);
		//  补充: 销毁信号量
		sem_destroy(t_task_sched->task_queue->resource);
		free(t_task_sched->task_queue->resource);

		free(t_task_sched->task_queue);
	}
	
	// 4. destroy pthread_t tids
	if (t_task_sched->tids) free(t_task_sched->tids);

	// 5. free it self
	free(t_task_sched);
}

tgx_task_t *tgx_task_init(void)
{
	return (tgx_task_t *)calloc(1, sizeof(tgx_task_t));
}

int tgx_post_task(tgx_task_schedule_t *t_task_sched, tgx_task_t *new_task)
{
	int ret;

	if (!new_task) {
		log_err("null pointer\n");
		return -1;
	}

	{
		// ...........................
		// TODO: 这个区域检测其他不能post的原因， 目前还没有发现该写什么
		// 我们就是在post之前先系统内部合计一下， 再调用用户指定的回调处理
		// 直到一切ok， 再去post
	}

	if (new_task->task_lock) {
		ret = pthread_mutex_trylock(new_task->task_lock);
		if (ret < 0) {
			log_err("you can not post this task once again before done.\n");
			return -1;
		}
	}
	
	// 执行用户回调函数， 这里设计这个回调的原因是为了避免在执行CPU密集型计算时， 
	// 用户发送数据导致触发EPOLLIN事件， 造成task再次被post， 虽然我们有互斥量
	// 保证， 但是这可能造成了不必要的post_task, 这样用户可以在post_task时， 暂时
	// 从事件IO系统中删除对应的fd

	if (new_task->on_post) {
		ret = new_task->on_post(new_task->context);
		if (ret < 0) {
			log_err("post task error\n");
			return -1;
		}
	}

	__tgx_task_internal_t *t_task_internal;
	t_task_internal = calloc(1, sizeof(__tgx_task_internal_t));
	if (!t_task_internal) {
		log_err("null pointer\n");
		return -1;
	}

	t_task_internal->task = new_task;
	pthread_mutex_lock(t_task_sched->sched_lock);
	tgx_task_queue_signal(t_task_sched->task_queue);
	tgx_task_queue_push_back(t_task_sched->task_queue, t_task_internal);
	pthread_mutex_unlock(t_task_sched->sched_lock);
	
	return 0;
}

tgx_task_queue_t* tgx_task_queue_init(void)
{
	int ret;

	tgx_task_queue_t *t_task_queue;
	t_task_queue = calloc(1, sizeof(tgx_task_queue_t));
	if (!t_task_queue) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}

	t_task_queue->resource = calloc(1, sizeof(sem_t));
	if (!t_task_queue->resource) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}
	ret = sem_init(t_task_queue->resource, 0, 0);
	if (ret < 0) {
		log_err("sem_init():%s\n", strerror(errno));
		return NULL;
	}

	t_task_queue->n_task = 0;
	t_task_queue->head = NULL;
	t_task_queue->tail = NULL;

	return t_task_queue;
}

void tgx_task_queue_empty(tgx_task_queue_t *t_task_queue)
{
	int ret;
	tgx_task_t *task;

	if (!t_task_queue) return;

	// 不断从队列取出元素， 然后释放内存， 并且让资源 -= -1
	while ((task = tgx_task_queue_pop_front(t_task_queue))) {
		ret = sem_trywait(t_task_queue->resource);
		if (ret < 0) {
			log_err("something error.\n");
			return;
		}
		free(task);
	}

	t_task_queue->n_task = 0;
	t_task_queue->head = NULL;
	t_task_queue->tail = NULL;
}

int tgx_task_queue_push_back(tgx_task_queue_t *t_task_queue, __tgx_task_internal_t *t_task_internal)
{
	if (!t_task_queue || !t_task_internal) {
		log_err("null pointer\n");
		return -1;
	}

	// 一开始想用信号量表示task个数， 但是后来有点担心引入bug， 因为我如果忘了调用
	// 或少调用tgx_task_queue_signal， 那么这个信号就不准， 我必须保证队列本身不掺杂
	// 这些复杂的机制， 所有同步问题交给上层， 因此后来添加了一个n_task属性

	int sval;
	/*if (sem_getvalue(t_task_queue->resource, &sval) < 0) {
		log_err("sem_getvalue():%s\n", strerror(errno));
		return -1;
	}*/
	
	sval = t_task_queue->n_task;

	if (sval == 0) {
		t_task_queue->head = t_task_internal;
		t_task_queue->tail = t_task_internal;
	} else {
		t_task_internal->prev    = t_task_queue->tail;
		t_task_queue->tail->next = t_task_internal;
		t_task_queue->tail = t_task_internal;
	}
	t_task_queue->tail->next = NULL;
	t_task_queue->n_task++;

	return 0;
}

tgx_task_t* tgx_task_queue_pop_front(tgx_task_queue_t *t_task_queue)
{
	__tgx_task_internal_t *t_task_internal = NULL;
	tgx_task_t *task = NULL;
	
	if (!t_task_queue) {
		log_err("null pointer\n");
		return NULL;
	}

	int sval;
	sval = t_task_queue->n_task;

	// 一开始做成栈了， 先进后出
	/*if (sval == 0) {
		return NULL;
	} else if (sval == 1) {
		t_task_internal = t_task_queue->tail;
		t_task_queue->head = NULL;
		t_task_queue->tail = NULL;
	} else {
		t_task_internal = t_task_queue->tail;
		t_task_queue->tail = t_task_internal->prev;
		t_task_queue->tail->next = NULL;
	}*/

	// FIFO
	if (sval == 0) {
		return NULL;
	} else if (sval == 1) {
		t_task_internal = t_task_queue->tail;
		t_task_queue->head = NULL;
		t_task_queue->tail = NULL;
	} else {
		t_task_internal = t_task_queue->head;
		t_task_queue->head = t_task_internal->next;
	}
	
	t_task_queue->n_task--;
	task = t_task_internal->task;
	free(t_task_internal);

	return task;
}

int tgx_task_queue_signal(tgx_task_queue_t *t_task_queue)
{
	return sem_post(t_task_queue->resource);
}

int tgx_task_queue_wait(tgx_task_queue_t *t_task_queue)
{
	return sem_wait(t_task_queue->resource);
}
