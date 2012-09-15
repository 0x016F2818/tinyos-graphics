#include "tgx_http.h"

tgx_mime_type_t tgx_get_mime_type(char *str)
{
	int i;
	for (i = 0; tgx_mime_type_string[i] != NULL; i++)
		if (strcmp(tgx_mime_type_string[i], str) == 0)
			break;
	if (tgx_mime_type_string[i])
		return i;
	return -1;
}

tgx_http_parser_t* tgx_http_parser_init(tgx_cycle_t *tcycle)
{
	return NULL;
}

void tgx_http_parser_free(tgx_http_parser_t *t_http_p)
{

}

int tgx_http_parser_on_url_cb(http_parser * parser, const char *at,
				 size_t length)
{
	/* 在判断完是否为POST之后， 在每一个return -1处， 都做了一下事情
	 * 1. free(path)
	 * 2. conn->doc_fd = NOFILE // 这个主要用于区分服务器到底是发json还是发静态页面的判断条件
	 * 3. conn->http_status = HTTP_STATUS_XXX // XXX为各种异常的值， 这里使用了400和404
	 * 4. return -1
	 * 其实在send_response_header()中首先查看http_status, 如果为异常值， 是不会再去跳到下一个状态，
	 * 而是直接connection_close(conn), 因此决定下一个状态， 是由http_status和doc_fd两个决定的
	 * TODO: 可以指定一些异常的页面， 当出现异常时发送异常页面而不是野蛮的关闭连接如404.htm
	 */

	/*struct connection *conn = (void *)parser->data;
	if (!conn) {
		LOG("http_parser_on_path_callback():no data.\n");
		conn->http_status = HTTP_STATUS_400;
		return -1;
	}

	// 对于POST方法我们发送的是message， 因此这边不处理
	if (strcmp(http_method_str(parser->method), "POST") == 0) 
		return 0;

	// 为path分配空间， 我们必须将at中的数据移到path中
	char *path = calloc(length + 1, sizeof(char));
	if (!path) {
		LOG("calloc():%s\n", strerror(errno));
		conn->http_status = HTTP_STATUS_400;
		conn->doc_fd = NOFILE;
		return -1;
	}

	// 复制到path
	strncpy(path, at, length);
	path[length] = '\0';

	// 去除掉'?'
	char *ch;
	while ((ch = strchr(path, '?'))) *ch = '\0';

	// 去掉最后一个'/', 可以去除尾部多个问号, 但是保证不把最后一和/删除
	while ((ch = strrchr(path, '/'))) {
		if (ch == path + strlen(path) - 1 && ch != path)
			*ch = '\0';
		else
			break;
	}

	// 过滤掉不支持的服务器脚本语言
	|+if (strstr(path, ".php") || strstr(path, ".aspx")) {
		LOG("unsupport cgi error.\n");
		free(path);
		conn->http_status = HTTP_STATUS_400;
		conn->doc_fd = NOFILE;
		return -1;
	}+|

	// 如果path为目录， 需要自动加上index.html
	struct stat statbuf;
	if (stat(path, &statbuf) < 0) {
		LOG("path = %s\n", path);
		LOG("stat():%s\n", strerror(errno));
		free(path);
		conn->http_status = HTTP_STATUS_400;
		conn->doc_fd = NOFILE;
		return -1;
	}
	if (S_ISDIR(statbuf.st_mode)) {
		// 重新分配path空间， 刚刚好为加上strlen("/index.html") byte再加上'\0'
		char *tmp = realloc(path, strlen(path) + strlen("/index.html") + 1);

		// 保证即使realloc， path指针不丢失
		if (!tmp) {
			free(path);
			LOG("realloc():%s\n", strerror(errno));
			conn->doc_fd = NOFILE;
			conn->http_status = HTTP_STATUS_400;
			return -1;
		} else {
			path = tmp;
		}

		sprintf(path, "%s/index.html", path);

		// 阻止缓冲区溢出
		path[strlen(path)] = '\0';
	}

	LOG("path = %s\n", path);

	// 打开文件
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		free(path);
		conn->doc_fd = NOFILE;
		conn->http_status = HTTP_STATUS_404;
		LOG("open():%s\n", strerror(errno));
		return -1;
	}

	// 设置文件类型， 为发送文件做准备
	if (strstr(path, ".html") || strstr(path, ".htm"))
		conn->doc_type = MIME_TYPE_HTML;
	else if (strstr(path, ".jpeg"))
		conn->doc_type = MIME_TYPE_JPEG;
	else if (strstr(path, ".jpg"))
		conn->doc_type = MIME_TYPE_JPG;
	else if (strstr(path, ".png"))
		conn->doc_type = MIME_TYPE_PNG;
	else if (strstr(path, ".gif"))
		conn->doc_type = MIME_TYPE_GIF;
	else if (strstr(path, ".ico"))
		conn->doc_type = MIME_TYPE_ICO;
	else if (strstr(path, ".js"))
		conn->doc_type = MIME_TYPE_JAVASCRIPT;
	else if (strstr(path, ".css"))
		conn->doc_type = MIME_TYPE_CSS;
	else
		conn->doc_type = MIME_TYPE_PLAIN;

	conn->doc_fd = fd;
	conn->http_status = HTTP_STATUS_200;

	free(path);*/
	return 0;
}

int tgx_http_parser_on_header_field_cb(http_parser * parser, const char *at,
					 size_t length)
{
	/*struct connection *conn = (void *)parser->data;
	   if (!conn) {
	   LOG("http_parser_on_path_callback():no data.\n");
	   return -1;
	   }
	   if (conn->last_header_element != FIELD)
	   conn->num_headers++;

	   strncat(conn->headers[conn->num_headers-1][0], at, length);
	   conn->last_header_element = FIELD; */

	return 0;
}

int tgx_http_parser_on_header_value_cb(http_parser * parser, const char *at,
					 size_t length)
{
	/*struct connection *conn= parser->data;
	   if (!conn) {
	   LOG("http_parser_on_path_callback():no data.\n");
	   return -1;
	   }
	   strncat(conn->headers[conn->num_headers-1][1], at, length);
	   if (strcmp(conn->headers[conn->num_headers-1][0], "Content-Type") == 0) {
	   conn->doc_type = get_mime_type(conn->headers[conn->num_headers-1][1]);
	   printf("doc_type = %s\n", mime_type_string[conn->doc_type]);
	   }
	   conn->last_header_element = VALUE; */

	return 0;
}

int tgx_http_parser_on_header_complete_cb(http_parser * parser)
{
	return 0;
}

// 由于读body任务没有放在解析器中做， 这里body是不会回调的
int tgx_http_parser_on_body_cb(http_parser * parser, const char *at, size_t length)
{
	return 0;
}
