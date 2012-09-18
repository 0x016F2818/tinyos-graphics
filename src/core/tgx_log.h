#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"


/**********************************************/
/* DEBUG代码时用到, 在正式使用时， undef即可 */
/**********************************************/
#if defined(DEBUG_PRINT)
#define DEBUG(...) \
	do { \
		fprintf(stderr, "[DEBUG]:%s:%d:%s(): ", __FILE__, __LINE__, __FUNCTION__); \
		fprintf(stderr, __VA_ARGS__); \
	} while(0)
#else
void no_op();
#define DEBUG(...) no_op()
#endif /* NO_DEBUG */


/**********************************************/
/*打印信息，分为几个级别info/trace/warning/err*/
/**********************************************/

void log_print(int level, 
			   int use, 
			   const char *file, 
			   int line, 
			   const char *function, 
			   const char *format, 
			   ...);

#define log_info(...) \
	do { log_print(0, 1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); } while(0)

#define log_trace(...) \
	do { log_print(1, 1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); } while(0)

#define log_warning(...) \
	do { log_print(2, 1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); } while(0)

#define log_err(...) \
	do { log_print(3, 1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); } while(0)
