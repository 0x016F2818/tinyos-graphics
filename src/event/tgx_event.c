#include "tgx_event.h"

/*
 * static functions
 */

#define TGX_NO_INDEX_FIND -1

static int tgx_find_schedule_index(tgx_event_t *te, int fd)
{
	if (te->sched_array[fd] && te->sched_array[fd]->fd == fd) {
		return fd;
	} else {
		int i;
		for (i = 0; i < te->maxfds; i++) {
			if (te->sched_array[i] && te->sched_array[i]->fd == fd)
				return fd;
		}
		return TGX_NO_INDEX_FIND;
	}
	return TGX_NO_INDEX_FIND;
}


// 设计tgx_event目的是独立出事件处理的逻辑， 我们使用状态机负责思考fd下一步所处的状态，
// 状态切换的过程也是handler调整的过程， 因此event系统本身只需要在事件发生的时候查到相应
// 的数据结构， 然后类似回调的方式， 运行handler即可， 而且将event系统独立出来有一个好处，
// 我们可以任意切换我们想使用的异步方式， 可以是epoll, poll， select， 这些就是面向对象
// 中所说的变化点

tgx_event_t* tgx_event_init(tgx_cycle_t *tcycle, int maxfds)
{
	tgx_event_t *te;
	te = calloc(1, sizeof(*te));
	if (!te) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}
	// TODO:是否要将tgx_cycle_t设置到event系统中， 还不太清楚， 小心即可
	te->tcycle = tcycle;

	te->maxfds = maxfds;

	// usedfds用来统计当前的活跃连接数, 当服务器处理的连接数大于用于所设置的最大连接数
	// 时， TODO： 服务器的处理变为直接挂断， 这样可以保证服务器的稳定性
	te->usedfds = 0;

	te->sched_array = calloc(maxfds, sizeof(*te->sched_array));
	if (!te->sched_array) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}

	// 通过define， 可以随时更换引擎
#ifdef USE_LINUX_EPOLL
	if (tgx_linux_epoll_init(te) < 0) {
		log_err("reactor_linux_epoll_init():%s\n", strerror(errno));
		return NULL;
	}
#endif

	return te;
}

// destroy一般发生在SIGHUP信号或者服务器shutdown
void tgx_event_destroy (tgx_event_t *te)
{
	if (!te) {
		log_err("null pointer!\n");
		return;
	}
	if (te->free) te->free(te);

	int i;
	for (i = 0; i < te->maxfds; i++) {
		if (te->sched_array[i]) {
			free(te->sched_array[i]);
		}
	}
	free(te);
} 
 

int	tgx_event_schedule_register(tgx_event_t *te, 
								 int fd, 
								 tgx_handler_t handler,
								 void* context)
{
	if (!te) {
		log_err("null pointer...\n");
		return -1;
	}
	te->usedfds++;
	if (te->usedfds > te->maxfds - 1) {
		log_err("no more scheduled resource...\n");
		return -1;
	}

	// 注册的时候优先选择和fd向对应的那个坑， 如果发现坑已经被占用了， 那么回头再寻找
	// 这个是很可能发生的， 由于服务器进程除了需要打开客户端fd以外还需要打开文件，而
	// 文件fd是不会去占用坑的， 在大量并发的时候，可能有一部分坑不使用， 但是fd已经超过
	// maxfds， 此时usedfds < maxfds

	tgx_schedule_t *sched = calloc(1, sizeof(tgx_schedule_t));
	if (!sched) {
		log_err("null pointer...\n");
		return -1;
	}
	sched->fd = fd;
	sched->handler = handler;
	sched->context = context;

	if (te->sched_array[fd] == NULL) {
		te->sched_array[fd] = sched;
	} else {
		int i;
		for (i = 0; i < te->maxfds; i++)
			if (te->sched_array[i] == NULL) {
				te->sched_array[i] = sched;
				break;
			}
		// 其实这里不可能发生的, 上面已经判断过了
		if (i >= te->maxfds) {
			log_err("no more scheduled resource...\n");
			return -1;
		}
	}

	return 0;
}

// 我们没有将删除事件和unregister做到一起， 因此在使用
// unregister时请记住还需要delete event from event system
int tgx_event_schedule_unregister(tgx_event_t *te, int fd)
{
	if (!te) {
		log_err("null pointer\n");
		return -1;
	}

	if (fd > te->maxfds - 1 || fd < 0) {
		log_err("fd error\n");
		return -1;
	}

	int r_index;
	r_index = tgx_find_schedule_index(te, fd);
	if (r_index == TGX_NO_INDEX_FIND) {
		log_err("do not find fd.\n");
		return -1;
	} else {
		free(te->sched_array[r_index]);
		te->sched_array[r_index] = NULL;
	}

	te->usedfds--;
	return 0;
}

int	tgx_event_poll(tgx_event_t *te, int timeout_ms)
{
	if (!te) {
		log_err("null pointer.\n");
		return -1;
	}
	if (te->poll) {
		return te->poll(te, timeout_ms);
	} else {
		log_err("poll not found!\n");
		return -1;
	}

	return -1;
}

// tgx_event_ctl函数可以删除， 修改， 添加事件到event 系统中 
// 之所以让register和ctl分开设计， 这两个接口耦合在一起太复杂
int	tgx_event_ctl(tgx_event_t *te, int op, int fd, int event)
{
	if (!te) {
		log_err("null pointer\n");
		return -1;
 	}

	if (te->ctl)
		return te->ctl(te, op, fd, event);

	return -1;
} 

int	tgx_event_get_fd(tgx_event_t *te, int index)
{ 
	if (!te) {
		log_err("null pointer\n");
		return -1;
 	}

	if (te->get_fd)
		return te->get_fd(te, index);
 
	return -1;
} 
 
tgx_handler_t tgx_event_get_handler(tgx_event_t *te, int fd)
{ 
 	if (!te) {
		log_err("null pointer\n");
		return NULL;
 	}

	int r_index;
	r_index = tgx_find_schedule_index(te, fd);
	if (r_index == TGX_NO_INDEX_FIND)
		return NULL;
	else if (te->sched_array[r_index]->handler)
		return te->sched_array[r_index]->handler;

	log_err("no handler found.\n");
	return NULL;
} 



// context是可选的， 未来可能会支持， 默认为NULL， 也就是不修改context
int tgx_event_set_handler(tgx_event_t *te, int fd, tgx_handler_t handler, void *context)
{
	if (!te) {
		log_err("null pointer\n");
		return -1;
 	}

	int r_index;
	r_index = tgx_find_schedule_index(te, fd);
	if (r_index == TGX_NO_INDEX_FIND)
		return -1;
	else if (te->sched_array[r_index]->handler) {
		te->sched_array[r_index]->handler = handler;

		// TODO：这里有点问题， context指针指向新的空间， 老的空间谁来释放?
		// 如果在这里释放， 可是这是void *类型， 我们对这快空间内部一
		// 无所知, 而留给外面释放， 那么必然导致程序的逻辑混乱, 实际上我们还
		// 用不着去改参数

		/*if (context) te->sched_array[r_index]->context = context;*/
		if (context) {
			log_warning("now, The interface do not support set context"
					"sorry.It may be supported in the future.\n");
			return -1;
		}
		return 0;
	}
	return -1;
}

void* tgx_event_get_context(tgx_event_t *te, int fd)
{
	if (!te) {
		log_err("null pointer.\n");
		return NULL;
 	}

	int r_index;
	r_index = tgx_find_schedule_index(te, fd);
	if (r_index == TGX_NO_INDEX_FIND)
		return NULL;
	else if (te->sched_array[r_index]->context)
		return te->sched_array[r_index]->context;

	log_err("no context found.\n");
	return NULL;
}

int	tgx_event_get_event(tgx_event_t *te, int index)
{
	if (!te) {
		log_err("null pointer.\n");
		return -1;
 	}
	
	if (te->get_event)
		return te->get_event(te, index);

	log_err("no event found.\n");
	return -1;
}

int tgx_event_get_next_index(tgx_event_t *te, int index)
{
	if (!te) {
		log_err("null pointer.\n");
		return -1;
 	}

	if (te->get_next_index)
		return te->get_next_index(te, index);

	log_err("no index found.\n");
	return -1;
}
