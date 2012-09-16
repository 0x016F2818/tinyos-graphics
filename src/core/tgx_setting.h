#pragma once

// 和程序功能有关的编译选项
#define USE_GARBAGE_COLLECTION
#define USE_LINUX_EPOLL
#define USE_SOCKET_REUSEADDR 
#define USE_TCP_CORK

#define MAX_BUFFER_SIZE			65535
#define INITIAL_BUFFER_SIZE     100   // 初始长度 
#define INCR_FACTOR             2     // 增长因子
#define INCR_LENGTH				500   // 增长长度
#define BUFFER_NEARLY_BOUNDRY   10    // 边界距离， 离边界还有多远开始扩容

// 服务器的默认配置参数
#define TGX_PORT				9000
#define TGX_ROOT				"./www"
#define TGX_LOG_FILE			"./tgx.log"
#define TGX_LOCK_FILE			"./tgx.pid"
#define TGX_CONFIG_FILE			"./tgx.conf"
#define TGX_MAX_CONNECTION		10000
#define RFC1123_DATE_FMT "%a, %d %b %Y %H:%M:%S %Z"


#define TGX_LOCKMODE			(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
