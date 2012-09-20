#include "tgx_connection.h"


pthread_mutex_t tcycle_lock = PTHREAD_MUTEX_INITIALIZER;


/*****************************************************************
 发生错误时:
 注意tgx_close_connection()调用要小心
 不要发生在各种辅助函数中， 否则将会造成重复close的问题
 具体说明为:
 1. handler家族都是可以直接处理close的
 2. 我们注意到post_task出现在
			1) handler家族中read handler中
			2) 在task家族本身的处理链条中， 表现为on_complete中
    因此post家族的on_complete是可以处理close的

 总结:handler和on_complete函数是可以处理close的， 其他的
 只需要return -1即可
******************************************************************/

// Helper Function
static int _string_check_boundry(tgx_string_t *str, int boundry)
{

#define MAX_BUFFER_SIZE			65535
#define INITIAL_BUFFER_SIZE     100   // 初始长度 
#define INCR_FACTOR             2     // 增长因子
#define INCR_LENGTH				500   // 增长长度
#define BUFFER_NEARLY_BOUNDRY   10    // 边界距离， 离边界还有多远开始扩容

	if (!str || boundry < 0) return -1;
	// 给缓冲区分配空间， 并且让缓冲区动态增长
	if (str->size == 0) {
		/*str->size = INITIAL_BUFFER_SIZE;*/
		str->size = MAX_BUFFER_SIZE;
		str->data = (char *)calloc(str->size, sizeof(char));
		if (!str->data) {
			log_err("calloc():%s\n", strerror(errno));
			return -1;
		}
	}

	if (boundry > str->size - BUFFER_NEARLY_BOUNDRY && str->size < MAX_BUFFER_SIZE) {
			str->size += INCR_FACTOR * INCR_LENGTH * sizeof(char);
			char *p = NULL;
			p = realloc(str->data, str->size);
			if (!p) {
				log_err("realloc():%s\n", strerror(errno));
				return -1;
			}
			str->data = p;
	}

	return 0;
}

/*****************************************************************
 处理http分为两种
 1. handler, 处理IO, 它们和IO多路转换紧密的结合在一起
 2. task, 每一个task对应一个do， on_complete, 处理CPU密集型计算, 
    它们和任务调度器紧密的结合在一起
 这样就将epoll和线程池的优势都发挥出来了, 竭力避免阻塞， 是最高的宗旨

 由于task只能传入一个参数， 为了让tcycle，tconn都能被使用
 于是包装了一下传入task, 这就是为什么会有_task_context_wrapper_t结构
******************************************************************/

// task wrapper
typedef struct {
	tgx_cycle_t	     *tcycle;
	tgx_connection_t *tconn;
	void			 *context;
} _task_context_wrapper_t;

static int _post_task(tgx_cycle_t *tcycle, tgx_connection_t *tconn,
		int (*on_post)(void *context),
		int (*task_handler)(void *context),
		int (*on_complete)(void *context, int err)
		);

// start travelling
static int _task_start_travelling               (void *context);

// task 1
static int _task_http_parser_do                 (void *context);
static int _task_http_parser_on_complete        (void *context, int err);

// task 2
static int _task_get_prepare_write_on_post      (void *context);
static int _task_get_prepare_write_do           (void *context);

// task 3
static int _task_post_prepare_module_on_post    (void *context);
static int _task_post_prepare_module_do         (void *context);
static int _task_post_prepare_module_on_complete(void *context, int err);

// task 4
static int _task_post_prepare_write_on_post     (void *context);
static int _task_post_prepare_write_do          (void *context);

// end travelling
static int _task_end_travelling                 (void *context, int err);


static int tgx_connection_handler_write_header            (tgx_cycle_t *tcycle, void *context, int event);
static int tgx_connection_handler_get_method_write_file   (tgx_cycle_t *tcycle, void *context, int event);
static int tgx_connection_handler_post_method_write_buffer(tgx_cycle_t *tcycle, void *context, int event);
static int _tgx_connnection_handler_no_operation          (tgx_cycle_t *tcycle, void *context, int event);

tgx_connection_t *tgx_new_connection(tgx_cycle_t *tcycle, int fd)
{
	tgx_connection_t *tconn = (tgx_connection_t *)calloc(1, sizeof(tgx_connection_t));
	if (!tconn) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}

	tconn->fd                  = fd;
	tconn->httpReqBuf     = calloc(1, sizeof(tgx_string_t));
	tconn->httpReqBody    = calloc(1, sizeof(tgx_string_t));
	tconn->httpRespHeader = calloc(1, sizeof(tgx_string_t));
	tconn->httpRespBody   = calloc(1, sizeof(tgx_string_t));
	tconn->lock = calloc(1, sizeof(pthread_mutex_t));
	tconn->http_parser = calloc(1, sizeof(struct tgx_http_parser_s));

	if (tconn->httpReqBuf == NULL || tconn->httpRespHeader == NULL || 
			tconn->httpRespBody == NULL || tconn->lock == NULL ||
			tconn->http_parser == NULL) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}

	pthread_mutex_init(tconn->lock, NULL);
	tconn->rw_pos = 0;
	tconn->dlopen_handle = NULL;

	return tconn;
}

void tgx_free_connection(tgx_connection_t *tconn)
{
#define __FREE_TGX_STRING__(x) \
	if (x) { \
		if (x->data) free(x->data);\
		free(x);\
	}
	
	if (!tconn) return;
	__FREE_TGX_STRING__(tconn->httpReqBuf);
	__FREE_TGX_STRING__(tconn->httpReqBody);
	__FREE_TGX_STRING__(tconn->httpRespHeader);
	__FREE_TGX_STRING__(tconn->httpRespBody);
	if (tconn->lock) {
		pthread_mutex_destroy(tconn->lock);
		free(tconn->lock);
	}

	if (tconn->fd > 0) close(tconn->fd);
	if (tconn->http_parser) free(tconn->http_parser);
	if (tconn->dlopen_handle) dlclose(tconn->dlopen_handle);
	free(tconn);
#undef __FREE_TGX_STRING__
}

#define __CHECK_HANDLER_ARG_LIGAL(x) \
	if (!tcycle || !context) { \
		log_err("null pointer\n"); \
		return x;\
	}

void tgx_close_connection            (tgx_cycle_t *tcycle, tgx_connection_t *tconn)
{
	if (!tcycle || !tconn) return;

	/*pthread_mutex_lock(&tcycle_lock);*/
	tgx_event_schedule_unregister(tcycle->tevent, tconn->fd);
	/*pthread_mutex_unlock(&tcycle_lock);*/
	tgx_event_ctl(tcycle->tevent, TGX_EVENT_CTL_DEL, tconn->fd, 0);
	tgx_free_connection(tconn);
}

int tgx_connection_handler_read             (tgx_cycle_t *tcycle, void *context, int event)
{
	int ret;

	__CHECK_HANDLER_ARG_LIGAL(-1);
	tgx_connection_t *tconn = (tgx_connection_t *)context;
	if (!(event & TGX_EVENT_IN)) {
		tgx_close_connection(tcycle, tconn);
		return -1;
	}

	int nRead;
	do {
		_string_check_boundry(tconn->httpReqBuf, tconn->rw_pos);

		nRead = read(tconn->fd, tconn->httpReqBuf->data + tconn->rw_pos, 
				tconn->httpReqBuf->size - tconn->rw_pos);
		if (nRead == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else {
				log_err("read():%s\n", strerror(errno));
				tgx_close_connection(tcycle, tconn);
				return -1;
			}
		} else if (nRead == 0) { 
			log_err("connection close fd = %d\n", tconn->fd);
			tgx_close_connection(tcycle, tconn);
			return -1;
		} else if (nRead > 0) {
			tconn->rw_pos += nRead;
		}
	} while (1);
	
	/*******************************************/
	// 避免缺少\0下标越界
	tconn->httpReqBuf->data[tconn->rw_pos] = '\0';
	/*******************************************/

	if (strstr(tconn->httpReqBuf->data, "\r\n\r\n") != NULL) {
		//  对buff进行分析， 如果是GET方法说明已经读取完毕, 如果是POST方法我们分析
		//  Content-Length， 拿到body长度， 再决定下一步是post task还是继续读取body

		// TODO: 这里没有做缓冲区检查， 要考虑一下
		if (strncmp(tconn->httpReqBuf->data, "GET", 3) == 0) {
			DEBUG("now get method post http parser task\n");
			ret = _post_task(tcycle, tconn, 
					_task_start_travelling,
					_task_http_parser_do,
					_task_http_parser_on_complete
					);
			if (ret < 0) {
				log_err("post http parser task error\n");
				tgx_close_connection(tcycle, tconn);
				return -1;
			}
			return 0;
		} else if (strncmp(tconn->httpReqBuf->data, "POST", 4) == 0) {


			// 这段代码块用来将Content-Length的值取出来和rw_pos对比
			{

				int header_len = 0;

				char num[16];
				char *start = strstr(tconn->httpReqBuf->data, "Content-Length:");
				char *end, *p;
				if (!start) {
					tgx_close_connection(tcycle, tconn);
					return -1;
				}
				start += 15;
				while (*start == ' ') start++;

				end = start;
				while (*end != '\r') end++;

				int i;
				for (p = start, i = 0; p < end; p++, i++)
					num[i] = *p;
				num[i] = '\0';
				
				p = strstr(tconn->httpReqBuf->data, "\r\n\r\n");
				header_len = p - tconn->httpReqBuf->data + 4;
				
				// 如果Content-Length大于目前读取的长度那么继续
				// 否则_post_task_post_prepare_module
				if (tconn->rw_pos > atoi(num) + header_len) {
					return 0;
				} else {
					ret = _post_task(tcycle, tconn, 
							_task_start_travelling,
							_task_http_parser_do,
							_task_http_parser_on_complete
							);
					if (ret < 0) {
						log_err("post http parser task error\n");
						tgx_close_connection(tcycle, tconn);
						return -1;
					}
					return 0;
				}
			}
		}
	}
	
	return 0;
}

int tgx_connection_handler_write_header     (tgx_cycle_t *tcycle, void *context, int event)
{
	__CHECK_HANDLER_ARG_LIGAL(-1);
	tgx_connection_t *tconn = (tgx_connection_t *)context;
	DEBUG("now write header:\n%s\n", tconn->httpRespHeader->data);

	if (!(event & TGX_EVENT_OUT)) {
		tgx_close_connection(tcycle, tconn);
		return -1;
	}

	DEBUG("\n");
	int nWrite;
	int headerLength = strlen(tconn->httpRespHeader->data);
	while (1) {
		DEBUG("\n");
		nWrite =  write(tconn->fd, tconn->httpRespHeader->data + tconn->rw_pos, headerLength - tconn->rw_pos);
		tconn->rw_pos += nWrite;
		if (nWrite == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {	//缓冲区写满了， 等会事件还会再来的, 先return
				DEBUG("\n");
				return 0;
			}
			log_err("write():%s\n", strerror(errno));
			DEBUG("rw_pos = %d\n", tconn->rw_pos);
			tgx_close_connection(tcycle, tconn);
			return -1;
		}
		if (tconn->rw_pos == headerLength) {
			// 不同的http_method转发到不同的writer handler
			DEBUG("\n");
			tconn->rw_pos = 0;
			if (strncmp(tconn->http_parser->http_method, "GET", 3) == 0) {
				DEBUG("\n");
				tgx_event_set_handler(tcycle->tevent, 
									  tconn->fd, 
									  tgx_connection_handler_get_method_write_file, 
									  NULL);
				return 0;
			} else if (strncmp(tconn->http_parser->http_method, "POST", 4) == 0) {
				DEBUG("\n");
				tgx_event_set_handler(tcycle->tevent, 
									  tconn->fd,
									  tgx_connection_handler_post_method_write_buffer,
									  NULL);
				return 0;
			} else {
				// Other Support http method
				log_err("http method not supported yet.\n");
				tgx_close_connection(tcycle, tconn);
				return -1;
			}
		}
	}

	return 0;
}

int tgx_connection_handler_get_method_write_file   (tgx_cycle_t *tcycle, void *context, int event)
{
	__CHECK_HANDLER_ARG_LIGAL(-1);
	tgx_connection_t *tconn = (tgx_connection_t *)context;

	if (!(event & TGX_EVENT_OUT)) {
		tgx_close_connection(tcycle, tconn);
		return -1;
	}

	struct stat statbuf;
	if (stat(tconn->http_parser->path, &statbuf) < 0 ||
			!S_ISREG(statbuf.st_mode)) {
		log_err("fstat():%s\n", strerror(errno));
		tgx_close_connection(tcycle, tconn);
		return -1;
	}
	
	int file_fd = open(tconn->http_parser->path, O_RDONLY);
	if (file_fd < 0) {
		log_err("open():%s\n", strerror(errno));
		tgx_close_connection(tcycle, tconn);
		return -1;
	}

	int nWrite;
	int contentLength = statbuf.st_size;
	while (1) {
		off_t offset = tconn->rw_pos;
		nWrite = sendfile(tconn->fd, file_fd, &offset, contentLength - tconn->rw_pos);
		tconn->rw_pos = offset;
		if (nWrite == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) { //缓冲区写满了， 等会事件还会再来的, 先return
				close(file_fd);
				return 0;
			}
			log_err("sendfile():%s\n", strerror(errno));
			close(file_fd);
			tgx_close_connection(tcycle, tconn);
			return -1;
		}

		if (tconn->rw_pos == contentLength) {
			close(file_fd);
			tgx_close_connection(tcycle, tconn);
			return 0;
		}
	}

	close(file_fd);
	return 0;
}

int tgx_connection_handler_post_method_write_buffer  (tgx_cycle_t *tcycle, void *context, int event)
{

	__CHECK_HANDLER_ARG_LIGAL(-1);
	tgx_connection_t *tconn = (tgx_connection_t *)context;
	DEBUG("now write body:\n%s\n", tconn->httpRespBody->data);

	if (!(event & TGX_EVENT_OUT)) {
		tgx_close_connection(tcycle, tconn);
		return -1;
	}
	DEBUG("\n");

	int nWrite;
	int contentLength = strlen(tconn->httpRespBody->data);
	while (1) {
		nWrite =  write(tconn->fd, tconn->httpRespBody->data + tconn->rw_pos, contentLength - tconn->rw_pos);
		tconn->rw_pos += nWrite;
		DEBUG("rw_pos = %d, contentLength = %d\n", tconn->rw_pos, contentLength);
		if (nWrite == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {	//缓冲区写满了， 等会事件还会再来的, 先return
				DEBUG("\n");
				return 0;
			}
			log_err("write():%s\n", strerror(errno));
			tgx_close_connection(tcycle, tconn);
			return -1;
		}
		if (tconn->rw_pos == contentLength) {
			DEBUG("\n");
			tgx_close_connection(tcycle, tconn);
			return 0;
		}
	}

	return 0;
}

static int _post_task(tgx_cycle_t *tcycle, tgx_connection_t *tconn,
		int (*on_post)(void *context),
		int (*task_handler)(void *context),
		int (*on_complete)(void *context, int err)
		)
{

	// 构造task
	_task_context_wrapper_t *task_context = calloc(1, sizeof(_task_context_wrapper_t));
	if (!task_context) {
		log_err("calloc():%s\n", strerror(errno));
		return -1;
	}

	task_context->tcycle = tcycle;
	task_context->tconn = tconn;

	// task的释放在任务调度器执行完该task之后， 由系统完成释放
	tgx_task_t *task = tgx_task_init();
	if (!task) {
		log_err("tgx_task_init() error\n");
		return -1;
	}

	task->task_lock = NULL;
	task->on_post			= on_post;
	task->on_task_complete  = on_complete;
	task->task_handler		= task_handler;
	task->context = (void *)task_context;
	if (tgx_post_task(tcycle->task_sched, task) < 0) return -1;

	// 理论上task_context是可以释放的
	/*free(task_context);*/
	return 0;
}

static int _tgx_connnection_handler_no_operation          (tgx_cycle_t *tcycle, void *context, int event)
{
	return 0;
}

// start travelling
static int _task_start_travelling               (void *context)
{
	_task_context_wrapper_t *wrapper_context = (_task_context_wrapper_t *)context;

	if (!wrapper_context) {
		log_err("null pointer\n");
		return -1;
	}

	// 将fd从event系统中删除(但是资料在)， 这样就切断了和event系统的联系， 
	// fd将进入另外一个世界这里是cpu密集型计算的世界， fd将在线程池中畅游一番
	/*************************************/
	DEBUG("now start travelling\n");
	int ret1, ret2;
	wrapper_context->tconn->rw_pos = 0;
	ret1 = tgx_event_set_handler(wrapper_context->tcycle->tevent, 
			wrapper_context->tconn->fd,
			_tgx_connnection_handler_no_operation,
			NULL);

	ret2 = tgx_event_ctl(wrapper_context->tcycle->tevent,
			TGX_EVENT_CTL_DEL, 
			wrapper_context->tconn->fd,
			0);
	if (ret1 < 0 || ret2 < 0) {
		log_err("tgx_event_ctl() error\n");
		return -1;
	}
	/*************************************/
	return 0;
}

// task 1
static int _task_http_parser_do                 (void *context)
{

	_task_context_wrapper_t *wrapper_context = (_task_context_wrapper_t *)context;

	if (!wrapper_context) {
		return -1;
	}

	DEBUG("http parser...\n");
	struct http_parser_settings settings;
	memset(&settings, 0, sizeof(struct http_parser_settings));
	settings.on_url				 = tgx_http_parser_on_url_cb;
	settings.on_body			 = tgx_http_parser_on_body_cb;
	settings.on_header_field	 = tgx_http_parser_on_header_field_cb;
	settings.on_header_value	 = tgx_http_parser_on_header_value_cb;
	settings.on_headers_complete = tgx_http_parser_on_header_complete_cb;

	size_t nParsed;
	struct http_parser *parser = (struct http_parser *)calloc(1, sizeof(struct http_parser));
	if (!parser) {
		return -1;
	}

	http_parser_init(parser, HTTP_REQUEST);
	parser->data = (void *)wrapper_context->tconn;
	nParsed = http_parser_execute(parser, &settings, 
			wrapper_context->tconn->httpReqBuf->data, 
			strlen(wrapper_context->tconn->httpReqBuf->data));
	if (nParsed != strlen(wrapper_context->tconn->httpReqBuf->data)) {
		free(parser);
		return -1;
	}
	free(parser);

	return 0;
}

static int _task_http_parser_on_complete        (void *context, int err)
{

	DEBUG("http parser on complete, err = %d\n", err);
	_task_context_wrapper_t *wrapper_context = (_task_context_wrapper_t *)context;

	if (!wrapper_context) {
		return -1;
	}

	if (err < 0) {
		log_err("http_parser error\n");
		tgx_close_connection(wrapper_context->tcycle, wrapper_context->tconn);
		return -1;
	}

	// 根据解析结果决定是用get任务链条来处理， 还是用post任务链条来处理
	DEBUG("http-method:%s\n", wrapper_context->tconn->http_parser->http_method);
	int ret;
	if (strncmp(wrapper_context->tconn->http_parser->http_method, "GET", 3) == 0) {
		DEBUG("now post prepare write task\n");
		ret = _post_task(wrapper_context->tcycle, wrapper_context->tconn,
				_task_get_prepare_write_on_post,
				_task_get_prepare_write_do,
				_task_end_travelling
				);
		if (ret < 0) {
			log_err("http_parser error\n");
			tgx_close_connection(wrapper_context->tcycle, wrapper_context->tconn);
			return -1;
		}
		return 0;
	} else if (strncmp(wrapper_context->tconn->http_parser->http_method, "POST", 4) == 0) {
		DEBUG("now post task to prepare module\n");
		ret = _post_task(wrapper_context->tcycle, wrapper_context->tconn,
				_task_post_prepare_module_on_post,
				_task_post_prepare_module_do,
				_task_post_prepare_module_on_complete
				);
		if (ret < 0) {
			log_err("http_parser error\n");
			tgx_close_connection(wrapper_context->tcycle, wrapper_context->tconn);
			return -1;
		}
		return 0;
	} else {
		log_err("http method not supported!\n");
		tgx_close_connection(wrapper_context->tcycle, wrapper_context->tconn);
		return -1;
	}
	return 0;
}

// task 2
static int _task_get_prepare_write_on_post      (void *context)
{

	DEBUG("prepare write task on post\n");
	// 还没发现要处理什么
	return 0;
}

static int _task_get_prepare_write_do           (void *context)
{

	_task_context_wrapper_t *wrapper_context = (_task_context_wrapper_t *)context;

	if (!wrapper_context) {
		return -1;
	}

	DEBUG("prepare write task start doing\n");
	tgx_cycle_t *tcycle = wrapper_context->tcycle;
	tgx_connection_t *tconn = wrapper_context->tconn;

	// 到达这个task说明解析没有问题， 影响header的因素
	// 1. http_status
	// 2. path
	// http_status来自http_parser解析的结果， 而path根据http_status作修改
	__CHECK_HANDLER_ARG_LIGAL(-1);

	int buff_pos = 0;

	// memset buff
	if (tconn->httpRespHeader->size > 0)
		memset(tconn->httpRespHeader->data, 0, sizeof(tconn->httpRespHeader->data));

#undef COPY
#define COPY(...) \
	_string_check_boundry(tconn->httpRespHeader, buff_pos); \
	buff_pos += sprintf(tconn->httpRespHeader->data + buff_pos, __VA_ARGS__)

	COPY("%s\r\n", tgx_http_status_string[tconn->http_parser->http_status]);
	COPY("%s\r\n", "Server: tinyos-graphics/0.0.1");
	COPY("%s\r\n", "Connection: Close");

	// 如果status 不是200 那么根据状态的不同修改path, 到达write body
	// 函数时将无条件根据path直接发送数据
	/**************************************************************/
	if (tconn->http_parser->http_status != TGX_HTTP_STATUS_200) {

		// 404
		{
			if (tconn->http_parser->http_status == TGX_HTTP_STATUS_404) {

				log_err("404 err, Now response 404.html to browser.\n");
				strcpy(tconn->http_parser->path, tcycle->err_page.e_404);
			}
		}

		// TODO:其他的错误， 还没有实现
		{

		}
	}
	/***************************************************************/
	struct stat statbuf;
	if (stat(tconn->http_parser->path, &statbuf) < 0) {
		log_err("fstat():%s\n", strerror(errno));
		tgx_close_connection(tcycle, tconn);
		return -1;
	}

	// 304, 这个可以利用浏览器的缓存， 加快速度
	{
		char tempstring[256];
		struct tm tm_t;
		struct tm *tm;
		time_t tmt;
		tmt = time(NULL);
		tm = gmtime_r(&tmt, &tm_t);
		strftime(tempstring, sizeof(tempstring), RFC1123_DATE_FMT, tm);
		COPY("Date: ");
		COPY("%s\r\n",tempstring);

		// 写入文件修改时间
		tm = gmtime_r(&statbuf.st_mtime, &tm_t);
		strftime(tempstring, sizeof(tempstring), RFC1123_DATE_FMT, tm);
		COPY("Last-modified: ");
		COPY("%s\r\n", tempstring);
	}


	COPY("Content-Type: %s\r\n", tgx_mime_type_string[tconn->http_parser->mime_type]);
	COPY("Content-Length: %ld\r\n", statbuf.st_size);
	COPY("\r\n");

	// 避免缓冲区溢出
	tconn->httpRespHeader->data[buff_pos] = '\0';
#undef COPY

	return 0;	
}

// task 3
static int _task_post_prepare_module_on_post    (void *context)
{
	_task_context_wrapper_t *wrapper_context = (_task_context_wrapper_t *)context;

	if (!wrapper_context) {
		return -1;
	}

	int (*user_handler)(tgx_module_http_t *http);
	char *error;

	DEBUG("\n");
	wrapper_context->tconn->dlopen_handle = 
		dlopen(wrapper_context->tconn->http_parser->path, RTLD_LAZY | RTLD_LOCAL);
	if (!wrapper_context->tconn->dlopen_handle) {
		log_err("dlopen %s\n", dlerror());
		return -1;
	}

	dlerror();		/* Clear any existing error */

	user_handler = (int (*)(tgx_module_http_t *))
		dlsym(wrapper_context->tconn->dlopen_handle, "TGX_MODULE_HTTP_HANDLER");
	if ((error = dlerror()) != NULL) {
		log_err("dlopen()%s\n", error);
		return -1;
	}

	// 这里使用了一些淫技 --!
	wrapper_context->context = user_handler;

	return 0;
}

static int _task_post_prepare_module_do         (void *context)
{
	_task_context_wrapper_t *wrapper_context = (_task_context_wrapper_t *)context;

	if (!wrapper_context) {
		return -1;
	}

	DEBUG("\n");
	tgx_module_http_t *http = calloc(1, sizeof(tgx_module_http_t));
	if (!http) return -1;

	http->req  = wrapper_context->tconn->httpReqBuf;
	http->resp = wrapper_context->tconn->httpRespBody; 

	// 转化成函数指针， 执行
	int ret;
	ret = ((int (*)(tgx_module_http_t *))wrapper_context->context)(http);

	/*free(http);*/
	return ret;
}

static int _task_post_prepare_module_on_complete(void *context, int err)
{
	DEBUG("\n");
	_task_context_wrapper_t *wrapper_context = (_task_context_wrapper_t *)context;

	if (!wrapper_context) {
		return -1;
	}

	// 用户模块返回-1也算error的范围
	if (err < 0) {
		log_err("calling user module error\n");
		tgx_close_connection(wrapper_context->tcycle, wrapper_context->tconn);
		return -1;
	}

	int ret;
	ret = _post_task(wrapper_context->tcycle, wrapper_context->tconn,
		_task_post_prepare_write_on_post,
		_task_post_prepare_write_do,
		_task_end_travelling
		);
	if (ret < 0) {
		log_err("post task: prepare write wrror\n");
		tgx_close_connection(wrapper_context->tcycle, wrapper_context->tconn);
		return -1;
	}
	return 0;
}

// task 4
static int _task_post_prepare_write_on_post     (void *context)
{
	// 还没发现要处理什么
	DEBUG("\n");
	return 0;
}

static int _task_post_prepare_write_do          (void *context)
{

	DEBUG("\n");
	// 实际上用户已经填写好了body， 我们要做的就是根据body填写好一份
	// header即可
	_task_context_wrapper_t *wrapper_context = (_task_context_wrapper_t *)context;

	if (!wrapper_context) {
		return -1;
	}

	tgx_cycle_t *tcycle = wrapper_context->tcycle;
	tgx_connection_t *tconn = wrapper_context->tconn;

	__CHECK_HANDLER_ARG_LIGAL(-1);

	int buff_pos = 0;

	// memset buff
	if (tconn->httpRespHeader->size)
		memset(tconn->httpRespHeader->data, 0, sizeof(tconn->httpRespHeader->data));

#undef COPY
#define COPY(...) \
	_string_check_boundry(tconn->httpRespHeader, buff_pos); \
	buff_pos += sprintf(tconn->httpRespHeader->data + buff_pos, __VA_ARGS__)

	COPY("%s\r\n", tgx_http_status_string[tconn->http_parser->http_status]);
	COPY("%s\r\n", "Server: tinyos-graphics/0.0.1");
	COPY("%s\r\n", "Connection: Close");

	if (tconn->http_parser->http_status != TGX_HTTP_STATUS_200) return -1;

	// 一般情况下post方法的mime_type都会是plain/text
	COPY("Content-Type: %s\r\n", "application/json");

	/*COPY("Content-Type: %s\r\n", tgx_mime_type_string[tconn->http_parser->mime_type]);*/

	// 测试了一下即使size == strlen(data)， 也就是结尾没有\0， 完全填满了， strlen函数
	// 返回的结果也是正确的, 这正好符合要求
	COPY("Content-Length: %d\r\n", strlen(tconn->httpRespBody->data));
	COPY("\r\n");

	// 避免缓冲区溢出
	tconn->httpRespHeader->data[buff_pos] = '\0';
#undef COPY

	DEBUG("POST:prepare header\n%s\n", tconn->httpRespHeader->data);
	DEBUG("POST:prepare body\n%s\n", tconn->httpRespBody->data);
	return 0;	

}

// end travelling
static int _task_end_travelling                 (void *context, int err)
{
	_task_context_wrapper_t *wrapper_context = (_task_context_wrapper_t *)context;

	if (!wrapper_context) {
		return -1;
	}

	DEBUG("end travelling, err = %d\n", err);
	if (err < 0) {
		log_err("post method prepare_write error\n");
		tgx_close_connection(wrapper_context->tcycle, wrapper_context->tconn);
		return -1;
	}

	/****************************************************/
	// 对于GET/POST方法， 旅程结束， 下面将和event系统进行
	// 衔接，准备回到"现实世界", Welcome Back!
	int ret1, ret2;

	// 注意这里的顺序不能翻过来， 因为这个是在线程内执行的， 必须
	// 先将handler设置号， 再"衔接", 并且rw_pos也要先设置好!
	wrapper_context->tconn->rw_pos = 0;
	ret1 = tgx_event_set_handler(wrapper_context->tcycle->tevent, 
			wrapper_context->tconn->fd,
			tgx_connection_handler_write_header,
			NULL);
	ret2 = tgx_event_ctl(wrapper_context->tcycle->tevent, 
			TGX_EVENT_CTL_ADD, 
			wrapper_context->tconn->fd, 
			TGX_EVENT_OUT);
	if (ret1 < 0 || ret2 < 0) {
		log_err("return back to event notify system error\n");
		tgx_close_connection(wrapper_context->tcycle, wrapper_context->tconn);
		return -1;
	}
	free(wrapper_context);
	/****************************************************/
	return 0;
}
