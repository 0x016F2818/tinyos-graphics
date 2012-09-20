#include "tgx_http_parser.h"

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

int tgx_http_parser_on_url_cb(http_parser *parser, const char *at,
				 size_t length)
{

	DEBUG("req:\n%s\n", at);


	// 解析过程中遇到自己不支持或者找不到指定文件这些不属于解析错误，
	// 应该返回0, 同时设置TGX_HTTP_STATUS_XXX即可， 返回-1表示终止
	// 链接， 服务器将认为http request本身有问题


	tgx_connection_t *tconn = (void *)parser->data;
	if (!tconn) {
		log_err("null pointer\n");
		return -1;
	}

	// 目前只支持GET与POST
	// 判断GET方法还是POST方法
	if (strcmp(http_method_str(parser->method), "GET") == 0) {
		sprintf(tconn->http_parser->http_method, "GET");
	} else if (strcmp(http_method_str(parser->method), "POST") == 0) { 
		sprintf(tconn->http_parser->http_method, "POST");
	} else {
		tconn->http_parser->http_status = TGX_HTTP_STATUS_400;
		return 0;
	}

	// 为path分配空间， 我们必须将at中的数据移到path中
	char *path = calloc(length + 3, sizeof(char));
	if (!path) {
		log_err("calloc():%s\n", strerror(errno));
		tconn->http_parser->http_status = TGX_HTTP_STATUS_500;
		return 0;
	}

	// 复制到path
	snprintf(path, length+1, "%s", at);


	// 去除掉'?'
	char *ch;
	while ((ch = strchr(path, '?'))) *ch = '\0';

	// 去掉最后一个'/', 可以去除尾部多个问号, 但是保证不把最后一个/删除
	while ((ch = strrchr(path, '/'))) {
		if (ch == path + strlen(path) - 1 && ch != path)
			*ch = '\0';
		else
			break;
	}

	// 过滤掉不支持的服务器脚本语言
	char *p = NULL;

	// 由于c语言的短路规则， 因此只要有一个为真， 就执行下面的语句，
	// 这个逻辑是正确的
	if ((p = strstr(path, ".php")) || (p = strstr(path, ".aspx")) ||
			(p = strstr(path, ".jsp"))) {
		if (strstr(p, "/") == NULL) {
			log_err("unsupport cgi error.\n");
			free(path);
			tconn->http_parser->http_status = TGX_HTTP_STATUS_400;
			return 0;
		}
	}

	// path主要检查uri本身的错误
	// path的使命完成了， 将path拷贝到tconn->http_parser->path,
	// 加上当前目录.标志
	sprintf(tconn->http_parser->path, 
			".%s", 
			path);
	free(path);


	// 如果path为目录， 需要自动加上index.html
	struct stat statbuf;
	if (stat(tconn->http_parser->path, &statbuf) < 0) {
		log_err("path = %s, stat():%s\n", tconn->http_parser->path, strerror(errno));
		tconn->http_parser->http_status = TGX_HTTP_STATUS_404;
		return 0;
	}

	if (S_ISDIR(statbuf.st_mode)) {
		int find = 0;
		struct stat statbuf_tmp;
		char *path_tmp;
		path_tmp = calloc(1, sizeof(tconn->http_parser->path));
		if (!path_tmp) {
			log_err("calloc():%s\n", strerror(errno));
			return -1;
		}

		while (1) {
			// 1. 测试index.html是否存在
			sprintf(path_tmp, "%s/index.html", tconn->http_parser->path);
			if (stat(path_tmp, &statbuf_tmp) == 0) {
				find = 1;
				strcpy(tconn->http_parser->path, path_tmp);
				break;
			}

			// 2. 测试index.htm是否存在
			sprintf(path_tmp, "%s/index.htm", tconn->http_parser->path);
			if (stat(path_tmp, &statbuf_tmp) == 0) {
				find = 1;
				strcpy(tconn->http_parser->path, path_tmp);
				break;
			}

			// 避免死循环
			break;
		}
		free(path_tmp);
		if (!find) {
			tconn->http_parser->http_status = TGX_HTTP_STATUS_404;
			return 0;
		}
	}

	// 设置文件类型， 为发送文件做准备
	if (strstr(tconn->http_parser->path, ".html") || strstr(tconn->http_parser->path, ".htm"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_HTML;
	else if (strstr(tconn->http_parser->path, ".jpeg"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_JPEG;
	else if (strstr(tconn->http_parser->path, ".jpg"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_JPG;
	else if (strstr(tconn->http_parser->path, ".png"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_PNG;
	else if (strstr(tconn->http_parser->path, ".gif"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_GIF;
	else if (strstr(tconn->http_parser->path, ".ico"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_ICO;
	else if (strstr(tconn->http_parser->path, ".js"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_JAVASCRIPT;
	else if (strstr(tconn->http_parser->path, ".css"))
		tconn->http_parser->mime_type = TGX_MIME_TYPE_CSS;
	else
		tconn->http_parser->mime_type = TGX_MIME_TYPE_PLAIN;
	
	// http_parser填充特殊字段部分
	{
		if (strstr(tconn->http_parser->path, ".wsn")) 
			tconn->http_parser->mod_flag = WSN_MODULE;

		// if ...

		// if ...
	}

	tconn->http_parser->http_status = TGX_HTTP_STATUS_200;

	return 0;
}

int tgx_http_parser_on_header_field_cb(http_parser * parser, const char *at,
					 size_t length)
{
	tgx_connection_t *tconn = (tgx_connection_t *)parser->data;
	if (!tconn) {
		log_err("null pointer\n");
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
		log_err("null pointer\n");
		return -1;
    }
	strncat(tconn->http_parser->headers[tconn->http_parser->num_headers-1][1], at, length);
	tconn->http_parser->last_header_element = VALUE; 

	return 0;
}

int tgx_http_parser_on_header_complete_cb(http_parser * parser)
{

	tgx_connection_t *tconn = (tgx_connection_t *)parser->data;
	if (!tconn) {
		log_err("null pointer\n");
		return -1;
    }


	{
		struct stat statbuf;
		if (stat(tconn->http_parser->path, &statbuf) < 0) {
			log_err("stat():%s\n", strerror(errno));
			tconn->http_parser->http_status = TGX_HTTP_STATUS_404;
			return 0;
		}

		int	i;
		for (i = 0; i < tconn->http_parser->num_headers; i++) {

			// 304 功能
			{
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
	}

	return 0;
}

int tgx_http_parser_on_body_cb(http_parser * parser, const char *at, size_t length)
{
	tgx_connection_t *tconn = (tgx_connection_t *)parser->data;
	if (!tconn) {
		log_err("null pointer\n");
		return -1;
    }

	if (tconn->httpRespBody->size < length+1) {
		tconn->httpReqBody->data = calloc(length+1, sizeof(char));
		tconn->httpReqBody->size = length+1;
	} else {
		memset(tconn->httpReqBody->data, 0, sizeof(tconn->httpReqBody->data));
	}

	strncpy(tconn->httpReqBody->data, at, length);

	return 0;
}
