#include "tgx_event.h"

static void tgx_linux_epoll_event_free(tgx_event_t *te)
{
	if (te == NULL) {
		log_err("null pointer.\n");
		return;
	}
	if (te->epoll_fd > 0) {
		close(te->epoll_fd);
		te->epoll_fd = -1;
	}

	if (te->epoll_events) {
		free(te->epoll_events);
		te->epoll_events = NULL;
	}
	return;
}

static int tgx_linux_epoll_ctl(tgx_event_t *te, int op, int fd, int event_flag)
{
	if (te == NULL) {
		log_err("null pointer.\n");
		return -1;
	}
	if (te->epoll_fd < 0) {
		log_err("epoll err\n");
		return -1;
	}

	int local_op;
	switch (op) {
		case TGX_EVENT_CTL_ADD:
			local_op = EPOLL_CTL_ADD;
			break;
		case TGX_EVENT_CTL_MOD:
			local_op = EPOLL_CTL_MOD;
			break;
		case TGX_EVENT_CTL_DEL:
			local_op = EPOLL_CTL_DEL;
			break;
		default:
			log_err("event_ctl():operation do not support!\n");
			return -1;
	}

	struct epoll_event event;
	memset(&event, 0, sizeof(struct epoll_event));

	event.data.fd = fd;
	if (event_flag & TGX_EVENT_IN)  event.events |= EPOLLIN;
	if (event_flag & TGX_EVENT_OUT) event.events |= EPOLLOUT;
	/*event.events |= EPOLLET | EPOLLRDHUP; // 使用边沿触发, 并且监视客户端关闭事件*/

	if (epoll_ctl(te->epoll_fd, local_op, fd, &event) < 0) {
		log_err("epoll_ctl():%s\n", strerror(errno));
		return -1;
	}

	return 0;
}

static int tgx_linux_epoll_get_event(tgx_event_t *te, int index)
{

	if (te == NULL) {
		log_err("null pointer.\n");
		return -1;
	}
	if (te->epoll_fd < 0) {
		log_err("epoll err\n");
		return -1;
	}

	int event, ret = 0;
	event = te->epoll_events[index].events;
	if (event & EPOLLIN)    ret |= TGX_EVENT_IN;
	if (event & EPOLLOUT)   ret |= TGX_EVENT_OUT;
	if (event & EPOLLERR)   ret |= TGX_EVENT_ERR;
	if (event & EPOLLHUP)   ret |= TGX_EVENT_HUP;
	if (event & EPOLLPRI)   ret |= TGX_EVENT_PRI;
	if (event & EPOLLRDHUP) ret |= TGX_EVENT_RDHUP;

	return ret;
}

static int tgx_linux_epoll_get_fd(tgx_event_t *te, int index)
{
	if (index < 0) index = 0;
	if (te == NULL || te->epoll_events == NULL) {
		log_err("null pointer.\n");
		return -1;
	}
	if (te->epoll_fd < 0) {
		log_err("epoll err\n");
		return -1;
	}

	return te->epoll_events[index].data.fd;
}

static int tgx_linux_epoll_get_next_index(tgx_event_t *te, int index)
{
	return index = -1 ? 0 : index + 1;
}

static int tgx_linux_epoll_poll(tgx_event_t *te, int timeout_ms)
{
	if (te == NULL || te->epoll_events == NULL) {
		log_err("null pointer.\n");
	}
	if (te->epoll_fd < 0) {
		log_err("epoll err\n");
	}

	return epoll_wait(te->epoll_fd, te->epoll_events, te->maxfds, timeout_ms);
}

int tgx_linux_epoll_init(tgx_event_t *te)
{
#define SET(x) \
	te->x = tgx_linux_epoll_ ##x

	SET(event_free);
	SET(ctl);
	SET(get_event);
	SET(get_fd);
	SET(get_next_index);
	SET(poll);

	if ((te->epoll_fd = epoll_create1(0)) < 0) {
		log_err("epoll_create1():%s\n", strerror(errno));
		return -1;
	}

	// 在EXEC时候close(epoll_fd);
	/*if (fcntl(re->epoll_fd, F_SETFD, FD_CLOEXEC) < 0) {
		log_err("fcntl():FD_CLOEXEC:%s\n", strerror(errno));
		return -1;
	}*/

	te->epoll_events = calloc(te->maxfds, sizeof(struct epoll_event));
	if (!te->epoll_events) {
		log_err("calloc():%s\n", strerror(errno));
		exit(errno);
	}

	return 0;
}
