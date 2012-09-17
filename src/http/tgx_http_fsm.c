#include "tgx_http_fsm.h"

// 声明
typedef struct {
	tgx_cycle_t			*tcycle;
	tgx_connection_t	*tconn;
	tgx_module_http_t	*http;
	int (*user_handler)(tgx_module_http_t *http);
} tgx_task_module_wrapper_context_t;

static int module_on_post(void *context);
static int module_task_handler(void *context);
static int module_on_task_complete(void *context, int err);


void tgx_http_fsm_set_status(tgx_connection_t *tc, int status)
{
	tc->status = status;
}

int tgx_http_fsm_state_machine(tgx_cycle_t *tcycle, tgx_connection_t *tc)
{
	// 状态机工作在connection的整个生命周期内， 因此让状态机无阻塞成为了高并发量的
	// 关键， 打算设计一个task_queue， 使用一个专门的处理task的线程， 这样状态机内部
	// 只需要post task即可， 这样就可以不阻塞了, 但是随着状态的增多， task增多， 必然
	// 导致在队列中等待的task饥饿， 这可能要到后期测试过程中才能知道.

	switch (tc->status) {
		case TGX_STATUS_CONNECT:
		case TGX_STATUS_READ_REQUEST_HEADER:
			{
				// TODO:目前还不支持keep-alive, 因此这段代码是不会执行的
				// 在accept建立好连接套接字之后， 状态即被设置为connect，
				// 下一次状态设置在read_request_header内部, 于是进入状态机
				// 也就是下一个代码块
				tc->read_pos = tc->write_pos = 0;
				memset(tc->buffer->data, 0, sizeof(tc->buffer->data));
			}
			break;
		case TGX_STATUS_PARSING_REQUEST_HEADER:
			{
				// 第四个参数NULL表示不改变context
				tgx_event_set_handler(tcycle->tevent, tc->fd, 
						tgx_connection_parse_req_header,
						NULL);
					
				// 之所以这边没有给出第三个参数， 是因为一般来到状态机内部就不需要
				// 事件了， 状态机的调用必然发生在由event驱动系统出发的handler之内
				// 而这些事件都被处理了
				tgx_connection_parse_req_header(tcycle, (void *)tc, 0);
			}
			break;
		case TGX_STATUS_GET_SEND_RESPONSE_HEADER:
			{
				tc->read_pos = tc->write_pos = 0;
				memset(tc->buffer->data, 0, sizeof(tc->buffer->data));
				tgx_event_set_handler(tcycle->tevent, tc->fd,
						tgx_connection_get_send_resp_header,
						NULL);
				tgx_connection_get_send_resp_header(tcycle, (void *)tc, 0);
			}
			break;
		case TGX_STATUS_GET_SEND_RESPONSE_FILE:
			{
				tc->read_pos = tc->write_pos = 0;
				memset(tc->buffer->data, 0, sizeof(tc->buffer->data));
				tgx_event_set_handler(tcycle->tevent, tc->fd,
						tgx_connection_get_send_resp_file,
						NULL);
				tgx_event_ctl(tcycle->tevent, TGX_EVENT_CTL_MOD, tc->fd, TGX_EVENT_OUT);
				tgx_connection_get_send_resp_file(tcycle, (void *)tc, 0);
			}
			break;
		case TGX_STATUS_POST_READ_REQUEST_MESSAGE_BODY:
			{
				// 我们首先将buffer指针向前移动去掉\r\n\r\n之前
				// 并且将read_pos置为\r\n\r\n后面
				char *p = NULL;
				p = strstr(tc->buffer->data, "\r\n\r\n");
				if (p == NULL) {
					log_err("something is wrong..., I think it impossible happen\n");
					return -1;
				}

				// TODO：这个地方要十分注意， 是个错误发生点
				p += 4;
				// 将p后面buffer->data + buffer->size - p长度的字符拷贝到buffer起始位置
				int i, len;
				len = tc->buffer->data + tc->buffer->size - p;
				for (i = 0; i < len; i++)
					tc->buffer->data[i] = *(p + i);
				tc->read_pos = i;
				tgx_event_set_handler(tcycle->tevent, tc->fd,
						tgx_connection_post_read_req_message_body,
						NULL);
				tgx_connection_post_read_req_message_body(tcycle, (void *)tc, 0);
			}
			break;
		case TGX_STATUS_POST_PARSING_REQUEST_MESSAGE_BODY:
			{
				// TODO: 先每次解析都是调用dlopen， 后期这个地方要修改为
				// 先查找是否已经打开， 就是在tcycle中保存一个模块的数组
				// 这样很可能减小客户端请求的时间, 但也需要经过测试才可以

				int (*user_handler)(tgx_module_http_t *http);
				char *error;

				tc->dlopen_handle = dlopen(tc->http_parser->path, RTLD_LAZY | RTLD_LOCAL);
				if (!tc->dlopen_handle) {
					log_err("%s\n", dlerror());
					return -1;
				}

				dlerror();		/* Clear any existing error */

				user_handler = (int (*)(tgx_module_http_t *))
					dlsym(tc->dlopen_handle, "TGX_MODULE_HTTP_HANDLER");
				if ((error = dlerror()) != NULL) {
					log_err("%s\n", error);
					return -1;
				}

				// 填写将要post task的task结构体

				tgx_module_http_t *http = calloc(1, sizeof(tgx_module_http_t));
				if (!http) {
					log_err("calloc():%s\n", strerror(errno));
					return -1;
				}
				http->req  = tc->buffer;
				http->resp = tc->httpResponse;

				{
					// 这里构造了一个适配器， 用户就可以拿到它所关注的数据， 而不是其他
					// 在服务器内部跑的数据结构
					// 注意虽然这里的函数块内的指针生存期非常短， 但是使用堆分配，内存块
					// 并未被释放
					tgx_task_module_wrapper_context_t *task_context = 
						calloc(1, sizeof(tgx_task_module_wrapper_context_t));
					if (!task_context) {
						log_err("calloc():%s\n", strerror(errno));
						return -1;
					}

					task_context->tcycle = tcycle;
					task_context->tconn = tc;
					task_context->http = http;
					task_context->user_handler = user_handler;

					tgx_task_t *task = tgx_task_init();
					if (!task) {
						log_err("tgx_task_init() error\n");
						return -1;
					}
					task->task_lock = NULL;
					task->on_post			= module_on_post;
					task->on_task_complete  = module_on_task_complete;
					task->task_handler		= module_task_handler;
					task->context = (void *)task_context;
					if (tgx_post_task(tcycle->task_sched, task) < 0) return -1;
				}

			}
			break;
		case TGX_STATUS_POST_SEND_RESPONSE_HEADER:
			{

			}
			break;
		case TGX_STATUS_POST_SEND_RESPONSE_DATA:
			{

			}
			break;
		case TGX_STATUS_ERROR:
		case TGX_STATUS_CLOSE:
			{
				tgx_event_schedule_unregister(tcycle->tevent, tc->fd);
				tgx_event_ctl(tcycle->tevent, TGX_EVENT_CTL_DEL, tc->fd, 0);
				close(tc->fd);
				if (tc->http_parser->http_status != TGX_HTTP_STATUS_404)
					close(tc->http_parser->path_fd);
			}
			break;
	}

	return 0;
}

// ********************************************************
// 下面三个函数用在任务调度引擎上， module_on_post在post那一刻触发， 也就是说还没有入队列
// 主要是用来进行一些预处理， module_task_handler也就是执行函数了, module_on_task_complete
// 发生在任务执行完成以后， 用来发通知， 或者做收尾工作

static int module_on_post(void *context)
{
	// 一旦加入到任务调度器， 那么我们就需要暂时删除在event系统上
	// 监视的客户端套接字, 在module_on_task_complete中我们再次恢复， 
	// 这样就可以保证用一个用户无法post两次， 不过这个问题也可以
	// 通过锁来解决， 实际上任务调度器本身的task_lock字段就是来
	// 解决这个问题的
	tgx_task_module_wrapper_context_t *wrapper_context = (
			tgx_task_module_wrapper_context_t *)context;
	if (!wrapper_context) return -1;
	tgx_event_ctl(wrapper_context->tcycle->tevent, 
			TGX_EVENT_CTL_DEL, wrapper_context->tconn->fd, 0);
	
	return 0;
}

static int module_task_handler(void *context)
{
	tgx_task_module_wrapper_context_t *wrapper_context = (
			tgx_task_module_wrapper_context_t *)context;
	if (!wrapper_context) return -1;

	return wrapper_context->user_handler(wrapper_context->http);
}

static int module_on_task_complete(void *context, int err)
{

	tgx_task_module_wrapper_context_t *wrapper_context = (
			tgx_task_module_wrapper_context_t *)context;
		if (!wrapper_context) return -1;

	if (err < 0) {
		tgx_http_fsm_set_status(wrapper_context->tconn, TGX_STATUS_ERROR);
		tgx_http_fsm_state_machine(wrapper_context->tcycle, wrapper_context->tconn);
		return -1;
	}

	// 将fd再次添加到event系统， 并且将事件调整为监视写
	tgx_event_ctl(wrapper_context->tcycle->tevent, 
			TGX_EVENT_CTL_ADD, wrapper_context->tconn->fd, 0);
	tgx_event_ctl(wrapper_context->tcycle->tevent, 
			TGX_EVENT_CTL_MOD, wrapper_context->tconn->fd, TGX_EVENT_OUT);
	tgx_http_fsm_set_status(wrapper_context->tconn, TGX_STATUS_POST_SEND_RESPONSE_HEADER);
	tgx_http_fsm_state_machine(wrapper_context->tcycle, wrapper_context->tconn);
	return 0;
}
