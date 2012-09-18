#include "tgx_log.h"

void no_op() { }

void log_print(int level, int use, const char *file, int line, const char *function, const char *format, ...)
{
	va_list ap;

	if (level < 0 || !use)
		return;

	switch( level ) {
		case 0:
			printf("[INFO]:");
			break;
		case 1:
			printf("[TRACE]:");
			break;
		case 2:
			printf("[WARNING]:");
			break;
		case 3:
			printf("[ERROR]:");
			break;
		default:
			return;
		
	}
	printf("%s:%d:", file, line);
 	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}
