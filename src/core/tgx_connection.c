#include "tgx_connection.h"

tgx_connection_t *tgx_connection_init(tgx_cycle_t *tcycle, int fd)
{
	tgx_connection_t *tconn = (tgx_connection_t *)calloc(1, sizeof(tgx_connection_t));
	if (!tconn) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}

	tconn->fd                  = fd;
	tconn->status              = TGX_STATUS_CONNECT;
	tconn->buffer			   = (tgx_string_t *)calloc(1, sizeof(tgx_string_t));
	if (!tconn->buffer) {
		log_err("calloc():%s\n", strerror(errno));
		return NULL;
	}

	tconn->read_pos			   = 0;
	tconn->write_pos		   = 0;
	tconn->http_parser		   = tgx_http_parser_init(tcycle);
	if (!tconn->http_parser) {
		log_err("tgx_http_parser_init() error\n");
		return NULL;
	}

	return tconn;
}

void tgx_connection_free(tgx_connection_t *tconn)
{
	if (!tconn) return;
	if (tconn->http_parser) tgx_http_parser_free(tconn->http_parser);
	if (tconn->buffer) {
		if (tconn->buffer->data) free(tconn->buffer->data);
		free(tconn->buffer);
	}

	free(tconn);
}

// 下面是状态机用到的所有和连接有关的状态转换函数
// 其实主要是两条线:分别是GET和POST			
//										  "GET"
//										  /=> send header => send static file => close
//										 /
// connect -> read header -> parse header 
//									 	 \ "POST"
//										  \=> read body => parse body=> send header => send user data => close

// 其中parse body不在这里， 这是用户程序的模块， server只负责
// 将读到的body转发到用户模块, 基本上一个请求写一个模块即可
// TODO: handler返回值名称需要重新设计
// TODO: 后期可以将模块构建成链表， 让用户可以为一个请求定义一个filter

// 主干上的函数
int tgx_connection_read_req_header(tgx_cycle_t *tcycle, void *context, int event)
{
	
	tgx_connection_t *tconn = (tgx_connection_t *)context;
	if (!tconn) {
		log_err("null pointer\n");
		return -1;
	}

	if (event & TGX_EVENT_ERR || 
			event & TGX_EVENT_HUP || !(event & TGX_EVENT_IN)) {
		log_err("event happens something\n");
		tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
		tgx_http_fsm_state_machine(tcycle, tconn);
		return -1;
	}

	// 给缓冲区分配空间， 并且让缓冲区动态增长
	if (tconn->buffer->size == 0) {
		tconn->buffer->size = INITIAL_BUFFER_SIZE;
		tconn->buffer->data = (char *)calloc(tconn->buffer->size, sizeof(char));
		if (!tconn->buffer->data) {
			log_err("calloc():%s\n", strerror(errno));
			tgx_http_fsm_set_status(tconn, TGX_EVENT_ERR);
			tgx_http_fsm_state_machine(tcycle, tconn);
			return -1;
		}
	}
	
	int nRead;
	while (1) {

		// TODO：检查当前容量是否不够, 不过这也带来一个问题， 如果客户端不断发数据， 会
		// 造成很严重的内存问题， 这里的INCR_FACTOR为增长因子
		if (tconn->read_pos > tconn->buffer->size - BUFFER_NEARLY_BOUNDRY &&
				tconn->buffer->size < MAX_BUFFER_SIZE) {
			tconn->buffer->size += INCR_FACTOR * INCR_LENGTH * sizeof(char);

			char *p = NULL;
			p = realloc(tconn->buffer->data, tconn->buffer->size);
			if (!p) {
				log_err("realloc():%s\n", strerror(errno));
				tgx_http_fsm_set_status(tconn, TGX_EVENT_ERR);
				tgx_http_fsm_state_machine(tcycle, tconn);
				return -1;
			}
			tconn->buffer->data = p;
		}

		nRead = read(tconn->fd, tconn->buffer->data + tconn->read_pos, 
				tconn->buffer->size - tconn->read_pos);
		if (nRead == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// 发生这两个事件， 一般是网速太慢， 缓冲区读完了， 新数据还没有到
				// 要么是客户端已经发送完毕数据， 因此我们break， 根据http请求的头部
				// 特征， 来判断到底怎么回事
				break;
			} else {
				log_err("read():%s\n", strerror(errno));
				tgx_http_fsm_set_status(tconn, TGX_EVENT_ERR);
				tgx_http_fsm_state_machine(tcycle, tconn);
				return -1;
			}
		} else if (nRead == 0) { 
		  // nRead == 0说明客户端关闭了套接字， 但我们很有可能在event里面
		  // 已经捕获了， 但这不妨碍， 增加健壮性 
			log_err("connection close itself fd = %d\n", tconn->fd);
			tgx_http_fsm_set_status(tconn, TGX_STATUS_CLOSE);
			tgx_http_fsm_state_machine(tcycle, tconn);
			return -1;
		} else if (nRead > 0) {
			tconn->read_pos += nRead;
		}
	}

	// 避免缓冲区溢出
	tconn->buffer->data[tconn->buffer->size - 1] = '\0';
	
	// 如果读到\r\n说明此时读取message header的任务已经完成, 接下来应该是parse message header
	// 注意虽然读取到\r\n\r\n但是可能buffer中也包含message body， 我们在解析完header之后应该
	// 去掉header部分
	if (strstr(tconn->buffer->data, "\r\n\r\n") != NULL) {
		tgx_http_fsm_set_status(tconn, TGX_STATUS_PARSING_REQUEST_HEADER);
		tgx_http_fsm_state_machine(tcycle, tconn);
	}
	
	// 否则直接返回， 事件还会被触发的
	return 0;
}

int tgx_connection_parse_req_header(tgx_cycle_t *tcycle, void *context, int event)
{
	if (!context || !tcycle) {
		log_err("null pointer\n");
		return -1;
	}

	tgx_connection_t *tconn = (tgx_connection_t *)context;


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
		log_err("calloc():%s\n", strerror(errno));
		tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
		tgx_http_fsm_state_machine(tcycle, tconn);
		return -1;
	}

	DEBUG("\n");
	http_parser_init(parser, HTTP_REQUEST);
	parser->data = (void *)tconn;
	nParsed = http_parser_execute(parser, &settings, tconn->buffer->data, strlen(tconn->buffer->data));
	if (nParsed != strlen(tconn->buffer->data)) {
		free(parser);
		log_err("http parser error", tconn->fd);
		tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
		tgx_http_fsm_state_machine(tcycle, tconn);
		return -1;
	}
	free(parser);

	// TODO:先分析一下tconn->http_parser的结果， 然后决定下一个状态是什么

	// 这里设计的是一种被动的状态机， 由每一个执行流程内部决定下一个状态， 
	// 状态机负责根据当前设置的状态继续做当前状态该做的事情， 打算在后期
	// 换一个思路， 让状态机决定状态

	// 下一步， 我们先根据解析得到的http_status， 先进行一些“预处理”
	if (tconn->http_parser->http_status == TGX_HTTP_STATUS_200 || 
		tconn->http_parser->http_status == TGX_HTTP_STATUS_304 ||
		tconn->http_parser->http_status == TGX_HTTP_STATUS_404) {
		if (strcmp(tconn->http_parser->http_method, "GET") == 0) {
			tgx_http_fsm_set_status(tconn, TGX_STATUS_GET_SEND_RESPONSE_HEADER);
		} else if (strcmp(tconn->http_parser->http_method, "POST") == 0) {
			tgx_http_fsm_set_status(tconn, TGX_STATUS_POST_READ_REQUEST_MESSAGE_BODY);
		} else {
			log_err("http_method do not support.\n");
			tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
		}
	} else {
			tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
	}
	// 进入下一个状态
	tgx_http_fsm_state_machine(tcycle, tconn);

	return 0;
}

// get 处理函数
int tgx_connection_get_send_resp_header(tgx_cycle_t *tcycle, void *context, int event)
{
	tgx_connection_t *tconn = (tgx_connection_t *)context;
	if (!tconn) {
		log_err("null pointer\n");
		return -1;
	}

	// TODO：这里没有使用tgx_string_t
	char response_header[MAX_BUFFER_SIZE];
	int buff_pos = 0;

#undef COPY
#define COPY(...) \
	buff_pos += sprintf(response_header + buff_pos, __VA_ARGS__)

	COPY("%s\r\n", tgx_http_status_string[tconn->http_parser->http_status]);
	COPY("%s\r\n", "Server: tinyos-graphics/0.0.1");
	COPY("%s\r\n", "Connection: Close");

	// 针对404 err
	{
		if (tconn->http_parser->http_status == TGX_HTTP_STATUS_404) {
			tconn->http_parser->path_fd = tcycle->err_page.e_404;
			log_err("err page fd = %d\n", tcycle->err_page.e_404);
		}
	}


	// 我想对于所有的GET方法， 根据状态的不同， 都会返回相应的文件， 如果是错误的
	// 那么返回: 404.html, 等等, 当然304 Not Modified， 是不需要发送文件的
	if (tconn->http_parser->path_fd >= 0) {
		struct stat statbuf;
		if (fstat(tconn->http_parser->path_fd, &statbuf) < 0) {
			log_err("fstat():%s\n", strerror(errno));
			tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
			tgx_http_fsm_state_machine(tcycle, tconn);
			return -1;
		}

		{
			// 和304有关
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
	}

	// 再添加一行， header结束
	COPY("\r\n");

	// 避免缓冲区溢出
	response_header[sizeof(response_header)-1] = '\0';

#undef COPY

	DEBUG("\n");
	
	// 接下来， 发送已经填写好的response_header
	int nWrite;
	int resp_len = strlen(response_header);
	while (1) {
		nWrite =  write(tconn->fd, response_header + tconn->write_pos, resp_len - tconn->write_pos);
		tconn->write_pos += nWrite;
		if (nWrite == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {	//缓冲区写满了， 等会事件还会再来的, 先return
				return 0;
			}
			log_err("write():%s\n", strerror(errno));
			tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
			tgx_http_fsm_state_machine(tcycle, tconn);
			return -1;
		}
		if (tconn->write_pos == resp_len) {
			if (tconn->http_parser->http_status == TGX_HTTP_STATUS_200 ||
				tconn->http_parser->http_status == TGX_HTTP_STATUS_404) {
				tgx_http_fsm_set_status(tconn, TGX_STATUS_GET_SEND_RESPONSE_FILE);
				tgx_http_fsm_state_machine(tcycle, tconn);
				return 0;
			} else if (tconn->http_parser->http_status == TGX_HTTP_STATUS_304) {
				tgx_http_fsm_set_status(tconn, TGX_STATUS_CLOSE);
				tgx_http_fsm_state_machine(tcycle, tconn);
				return 0;
			}
		}
	}

	return 0;
}

int tgx_connection_get_send_resp_file(tgx_cycle_t *tcycle, void *context, int event)
{
	tgx_connection_t *tconn = (tgx_connection_t *)context;
	if (!tconn) {
		log_err("null pointer\n");
		return -1;
	}


	if (tconn->http_parser->path_fd < 0) {
		log_err("file error\n");
		tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
		tgx_http_fsm_state_machine(tcycle, tconn);
		return -1;
	}

	struct stat statbuf;
	if (fstat(tconn->http_parser->path_fd, &statbuf) < 0) {
		log_err("fstat():%s\n", strerror(errno));
		tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
		tgx_http_fsm_state_machine(tcycle, tconn);
		return -1;
	}


	int nWrite;
	// TODO: 不知道为什么， 直接将statbuf.st_size带入sendfile， 就会出现错误
	int total_length = statbuf.st_size;
	while (1) {
		// 使用read_pos记载当前文件传输到什么位置， 下一次读取的最大长度就相应的减小
		off_t offset = tconn->write_pos;
		nWrite = sendfile(tconn->fd, tconn->http_parser->path_fd, &offset, total_length - tconn->write_pos);
		tconn->write_pos = offset;
		if (nWrite == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) { //缓冲区写满了， 等会事件还会再来的, 先return
				return 0;
			}
			log_err("sendfile():%s\n", strerror(errno));
			tgx_http_fsm_set_status(tconn, TGX_STATUS_ERROR);
			tgx_http_fsm_state_machine(tcycle, tconn);
			return -1;
		}

		if (tconn->write_pos == total_length) {
 			tgx_http_fsm_set_status(tconn, TGX_STATUS_CLOSE);
			tgx_http_fsm_state_machine(tcycle, tconn);
			return 0;
		}
	}

	return 0;
} 

// post 处理函数
int tgx_connection_post_read_req_message_body(tgx_cycle_t *tcycle, void *context, int event)
{
	return 0;
}

int tgx_connection_post_send_resp_header(tgx_cycle_t *tcycle, void *context, int event)
{
	return 0;
}

int tgx_connection_post_send_resp_data(tgx_cycle_t *tcycle, void *context, int event)
{
	return 0;
}

// 收尾工作函数, 对于keepalive连接， 打算还是先close掉
int tgx_connection_close(tgx_cycle_t *tcycle, void *context, int event)
{
	tgx_connection_t *tconn = (tgx_connection_t *)tconn;
	
	tgx_event_schedule_unregister(tcycle->tevent, tconn->fd);
	tgx_event_ctl(tcycle->tevent, TGX_EVENT_CTL_DEL, tconn->fd, 0);
	tgx_connection_free(tconn);
	return 0;
}

int tgx_connection_error(tgx_cycle_t *tcycle, void *context, int event)
{
	tgx_connection_t *tconn = (tgx_connection_t *)tconn;
	if (!tconn) {
		log_err("null pointer\n");
		return -1;
	}
	
	log_err("Now In tgx_connection_error Handler...\n");
	tgx_http_fsm_set_status(tconn, TGX_STATUS_CLOSE);
	tgx_http_fsm_state_machine(tcycle, tconn);
	return 0;
}
