#include <unistd.h>

#define PIPE_READER 1
#define PIPE_WRITER 2
#define MAX_BUFF_LENGTH 1024

typedef struct pid_info {
	// sys info
	int     client_fd;
	pid_t   pid;
	int     pipe_r;  // pipe write
	int     pipe_w;  // pipe read

	// network info
	int     id;
	char    name[MAX_BUFF_LENGTH];

	// list info
	struct pid_info *next;
	struct pid_info *prev;
} pid_info_t;

typedef struct {
	pid_info_t *head;
	pid_info_t *tail;
	int        num_process;
} pid_vector_t;

int pid_vector_init(pid_vector_t *pv);
void pid_vector_destroy(pid_vector_t *pv);
int pid_vector_query_pipe(pid_vector_t *pv, int id, char *name, int flag);
int pid_vector_add(pid_vector_t *pv, pid_info_t *pinfo);
int pid_vector_del(pid_vector_t *pv, int id, char *name);
