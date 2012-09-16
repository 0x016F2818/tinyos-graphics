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
	if (!tcycle) {
		log_err("null pointer\n");
		return NULL;
	}

	tgx_http_parser_t *http_parser;
	// calloc()  已经将http_parser内部清空了
	http_parser = calloc(1, sizeof(tgx_http_parser_t));
	if (!http_parser) {
		log_err("null pointer\n");
		return NULL;
	}
	
	http_parser->path_fd	 = -1;
	http_parser->path		 = NULL;
	return http_parser;
}

void tgx_http_parser_free(tgx_http_parser_t *t_http_p)
{
	if (!t_http_p) {
		log_err("null pointer\n");
		return;
	}

	// 由于path的长度不好控制， 这里根据解析的结果动态的分配
	if (t_http_p->path) free(t_http_p->path);

	free(t_http_p);
}

int tgx_http_parser_on_url_cb(http_parser *parser, const char *at,
				 size_t length)
{
	// 要区分好服务器内部错误和用户请求错误之间的关系

	system("pwd");
	tgx_connection_t *tconn = (void *)parser->data;
	if (!tconn) {
		log_err("null pointer\n");
		return -1;
	}

	// 判断GET方法还是POST方法
	if (strcmp(http_method_str(parser->method), "GET") == 0) {
		sprintf(tconn->http_parser->http_method, "GET");
	} else if (strcmp(http_method_str(parser->method), "POST") == 0) { 
		sprintf(tconn->http_parser->http_method, "POST");
	} else {
		tconn->http_parser->http_status = TGX_HTTP_STATUS_400;
		tconn->http_parser->path_fd = -1;
		return 0;
	}


	// 为path分配空间， 我们必须将at中的数据移到path中
	char *path = calloc(length + 1, sizeof(char));
	if (!path) {
		log_err("calloc():%s\n", strerror(errno));
		tconn->http_parser->http_status = TGX_HTTP_STATUS_500;
		tconn->http_parser->path_fd = -1;
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
	if (strstr(path, ".php") || strstr(path, ".aspx")) {
		log_err("unsupport cgi error.\n");
		free(path);
		tconn->http_parser->http_status = TGX_HTTP_STATUS_400;
		tconn->http_parser->path_fd = -1;
		return 0;
	}

	// 如果path为目录， 需要自动加上index.html
	struct stat statbuf;
	if (stat(path, &statbuf) < 0) {
		log_err("path = %s, stat():%s\n", path, strerror(errno));
		free(path);
		tconn->http_parser->http_status = TGX_HTTP_STATUS_400;
		tconn->http_parser->path_fd = -1;
		return 0;
	}
	if (S_ISDIR(statbuf.st_mode)) {
		// 重新分配path空间， 刚刚好为加上strlen("/index.html") byte再加上'\0'
		char *tmp = realloc(path, strlen(path) + strlen("/index.html") + 1);

		// 保证即使realloc， path指针不丢失
		if (!tmp) {
			free(path);
			log_err("realloc():%s\n", strerror(errno));
			tconn->http_parser->path_fd = -1;
			tconn->http_parser->http_status = TGX_HTTP_STATUS_400;
			return -1;
		} else {
			path = tmp;
		}

		sprintf(path, "%s/index.html", path);

		// 阻止缓冲区溢出
		path[strlen(path)] = '\0';
	}

	// 打开文件
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		free(path);
		tconn->http_parser->path_fd = -1;
		tconn->http_parser->http_status = TGX_HTTP_STATUS_404;
		log_err("open():%s\n", strerror(errno));
		return -1;
	}

	// 设置文件类型， 为发送文件做准备
	if (strstr(path, ".html") || strstr(path, ".htm"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_HTML;
	else if (strstr(path, ".jpeg"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_JPEG;
	else if (strstr(path, ".jpg"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_JPG;
	else if (strstr(path, ".png"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_PNG;
	else if (strstr(path, ".gif"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_GIF;
	else if (strstr(path, ".ico"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_ICO;
	else if (strstr(path, ".js"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_JAVASCRIPT;
	else if (strstr(path, ".css"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_CSS;
	else
		tconn->http_parser->mime_type = TGX_MIME_TYPE_PLAIN;
	
	// 这个是专为tinyos项目设置的模块后缀
	if (strstr(path, ".wsn")) 
		tconn->http_parser->is_wsn_module = 1;

	tconn->http_parser->path_fd = fd;
	tconn->http_parser->http_status = TGX_HTTP_STATUS_200;

	/*free(path);*/
	// 对于path解析错误， 立刻free掉， 否则将path加入到解析结果中方便查阅

	tconn->http_parser->path = path;
	return 0;
}

// 通过下面两个回调将所有的值全部放入到tconn->http_parser
// 字段中， 然后在complete回调中进行“定制”
int tgx_http_parser_on_header_field_cb(http_parser * parser, const char *at,
					 size_t length)
{
	tgx_connection_t *tconn = (tgx_connection_t *)parser->data;
	if (!tconn) {
		log_err("http_parser_on_path_callback():no data.\n");
		return -1;
    }
	if (tconn->http_parser->last_header_element != FIELD)
		tconn->http_parser->num_headers++;

	strncat(tconn->http_parser->headers[tconn->http_parser->num_headers-1][0], at, length);
	tconn->http_parser->last_header_element = FIELD; 

	return 0;
}

int tgx_http_parser_on_header_value_cb(http_parser * parser, const char *at,
					 size_t length)
{
	tgx_connection_t *tconn = (tgx_connection_t *)parser->data;
	if (!tconn) {
		log_err("http_parser_on_path_callback():no data.\n");
		return -1;
	}
	strncat(tconn->http_parser->headers[tconn->http_parser->num_headers-1][1], at, length);
	tconn->http_parser->last_header_element = VALUE; 

	return 0;
}

int tgx_http_parser_on_header_complete_cb(http_parser * parser)
{

	tgx_connection_t *tconn = (tgx_connection_t *)parser->data;

	// 304 功能
	{
		if (tconn->http_parser->path_fd < 0) return 0;
		struct stat statbuf;
		if (fstat(tconn->http_parser->path_fd, &statbuf) < 0) {
			log_err("stat():%s\n", strerror(errno));
			tconn->http_parser->http_status = TGX_HTTP_STATUS_400;
			tconn->http_parser->path_fd = -1;
			return 0;
		}

		int	i;
		for (i = 0; i < tconn->http_parser->num_headers; i++) {
			if (strcmp(tconn->http_parser->headers[i][0], "If-Modified-Since") == 0) {
				char tempstring[256];
				int len = strlen(tconn->http_parser->headers[i][1]);
				strncpy(tempstring, tconn->http_parser->headers[i][1], len);
				tempstring[len] = '\0';
				struct tm tm;
				time_t t;
				strptime(tempstring, RFC1123_DATE_FMT, &tm);
				tzset();
				t = mktime(&tm);
				t -= timezone;
				gmtime_r(&t, &tm);
				if (t >= statbuf.st_mtime) {
					tconn->http_parser->http_status = TGX_HTTP_STATUS_304;
				}
			}
		}
	}


	return 0;
}

// 由于读body任务没有放在解析器中做， 这里body是不会回调的
int tgx_http_parser_on_body_cb(http_parser * parser, const char *at, size_t length)
{
	return 0;
}
